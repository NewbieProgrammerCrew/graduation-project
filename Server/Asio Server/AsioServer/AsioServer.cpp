#include "Global.h"
#include "Session.h"
#include "protocol.h"

atomic_int User_ID;

void Init_Server()
{
	_wsetlocale(LC_ALL, L"korean");
}

int GetNewClientID()
{
	if (User_ID >= MAX_USER) {
		cout << "MAX USER FULL\n";
		exit(-1);
	}
	return User_ID++;
}

class server
{
private:
	tcp::acceptor acceptor;
	tcp::socket socket;
	void do_accept()
	{
		acceptor.async_accept(socket, [this](boost::system::error_code ec) {
			if (!ec)
			{
				int p_id = GetNewClientID();
				clients[p_id] = std::make_shared<cSession>(std::move(socket), p_id);
				clients[p_id]->start();
				do_accept();
			}
			});
	}

public:
	server(boost::asio::io_context& io_service, int port)
		: acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
		socket(io_service)
	{
		do_accept();
	}
};

void worker_thread(boost::asio::io_context* service)
{
	service->run();
}

int main()
{
	boost::asio::io_context io_service;
	vector <thread > worker_threads;
	server s(io_service, PORT_NUM);

	Init_Server();

	for (auto i = 0; i < 4; i++) worker_threads.emplace_back(worker_thread, &io_service);
	for (auto& th : worker_threads) th.join();
}