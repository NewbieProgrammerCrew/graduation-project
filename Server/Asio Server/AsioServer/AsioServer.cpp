#include "Global.h"
#include "Session.h"
#include "protocol.h"

mutex Mutex;

concurrency::concurrent_unordered_set<int> AvailableUserIDs;
atomic_int NowUserNum;


int Get_New_ClientID()
{
	if (NowUserNum >= MAX_USER) {
		cout << "SERVER_FULL\n";
		exit(-1);
	}

	Mutex.lock();
	int newUserID = *AvailableUserIDs.begin();
	AvailableUserIDs.unsafe_erase(newUserID);
	Mutex.unlock();

	return newUserID;
}

class cServer
{
private:
	tcp::acceptor	acceptor;
	tcp::socket		socket;
	void do_Accept()
	{
		acceptor.async_accept(socket, [this](boost::system::error_code ec) {
			if (!ec)
			{
				int p_id = Get_New_ClientID();
				clients[p_id] = std::make_shared<cSession>(std::move(socket), p_id);
				clients[p_id]->start();
				do_Accept();
			}
			});
	}

public:
	cServer(boost::asio::io_context& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port)), socket(io_service)
	{
		do_Accept();
	}
};

void Worker_Thread(boost::asio::io_context* service)
{
	service->run();
}
void Init_Server()
{
	for (int i = 0; i < MAX_USER; ++i) {
		AvailableUserIDs.insert(i);
	}
}
int main()
{
	boost::asio::io_context ioService;
	vector<thread> workerThreads;
	
	Init_Server();

	cServer server(ioService, PORT_NUM);

	unsigned int numThreads = std::thread::hardware_concurrency();
	for (auto i = 0; i < numThreads; i++) workerThreads.emplace_back(Worker_Thread, &ioService);
	for (auto& th : workerThreads) th.join();
}