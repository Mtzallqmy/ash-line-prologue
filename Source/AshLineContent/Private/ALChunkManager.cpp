#include "ALChunkManager.h"
int32 UALChunkManager::FindChunkForPackage(FName PackageId) const { for (const FALChunkDefinition& Chunk : Chunks) if (Chunk.PackageId == PackageId) return Chunk.ChunkId; return INDEX_NONE; }
