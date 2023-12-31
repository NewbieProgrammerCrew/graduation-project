// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainActor.h"
#include <cmath>
// Sets default values
AProceduralTerrainActor::AProceduralTerrainActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProceduralTerrainActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralTerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FVector> AProceduralTerrainActor::GenerateVertices(int size)
{
	for (int y = 0; y <= size; ++y) {
		for (int x = 0; x <= size; ++x) {
            m_Vertices.Add(FVector(x, y, 0));
		}
	}
   return m_Vertices;
}

TArray<int> AProceduralTerrainActor::GenerateTriangels(int size)
{
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int topLeftIndex = y * (size + 1) + x;

            // Ã¹ ¹øÂ° »ï°¢Çü ÀÎµ¦½º
            m_Triangles.Add(topLeftIndex);
            m_Triangles.Add(topLeftIndex + size+ 1);
            m_Triangles.Add(topLeftIndex + size + 2);

            // µÎ ¹øÂ° »ï°¢Çü
            m_Triangles.Add(topLeftIndex);
            m_Triangles.Add(topLeftIndex + size + 2);
            m_Triangles.Add(topLeftIndex + 1);
        }
    }
	return m_Triangles;
}

TArray<FVector2D>AProceduralTerrainActor:: GenerateUV(int size)
{  
    for (int y = 0; y <= size; ++y) {
        for (int x = 0; x <= size; ++x) {
            float u = fmod(x, 2);
            float v = fmod(y, 2);

            m_UV.Add(FVector2D(u, v));
        }
    }
    return m_UV;
}

void AProceduralTerrainActor::UpdateMeshSection(TArray<FVector> vertices)
{
    m_Vertices = vertices;
    TArray<FVector> Normals;
    TArray<FColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;

    MyMeshComponent->UpdateMeshSection(0, m_Vertices, Normals, m_UV, VertexColors, Tangents);
}

