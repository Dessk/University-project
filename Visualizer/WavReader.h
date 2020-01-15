#pragma once



typedef struct {
	unsigned char ChunkID[4];
	unsigned int ChunkSize;
	unsigned char Format[4];
	unsigned char SubChunk1ID[4];
	unsigned int SubChunk1Size;
	unsigned short int AudioFormat;
	unsigned short int NumChannels;
	unsigned int SampleRate;
	unsigned int ByteRate;
	unsigned short int BlockAlign;
	unsigned short int BitsPerSample;
	unsigned char SubChunk2ID[4];
	unsigned int SubChunk2Size;

	unsigned char DumpedChunkID[4];
	unsigned int DumpedChunkSize;

}header;

typedef struct {
	double* data;
	int size;
}WavData;



void ShowInfoWav(header head) {
	printf("\nChunkID: %.4s\nSize of file: %d bytes\nFormat: %.4s\nSubChunk1D: %.4s\nSubChunk1Size: %d\n\
AudioFormat: %d\nNumChannels: %d\nSampleRate:%d\nByteRate: %d\nBlockAlign: %d\nBitsPerSample: %d\nSubChunk2ID: %.4s\nSubChunk2Size: %d\n\n========DUMPED INFO========\n\
DumpedChunkID: %.4s\nDumpedChunkSize: %d\n\n",
head.ChunkID, head.ChunkSize, head.Format, head.SubChunk1ID, head.SubChunk1Size, head.AudioFormat,
head.NumChannels, head.SampleRate, head.ByteRate, head.BlockAlign, head.BitsPerSample, head.SubChunk2ID, head.SubChunk2Size, head.DumpedChunkID, head.DumpedChunkSize);
}

FILE* ReadWAV(char* name) {

	FILE* file;
	header test;

	if ((file = fopen(name, "rb")) != NULL) {
		printf("\nFile correctly loaded.\n");

		fread(test.ChunkID, 1, 4, file);
		fread(&test.ChunkSize, 4, 1, file);
		fread(test.Format, 1, 4, file);
		fread(test.SubChunk1ID, 1, 4, file);
		fread(&test.SubChunk1Size, 4, 1, file);
		fread(&test.AudioFormat, 2, 1, file);
		fread(&test.NumChannels, 2, 1, file);
		fread(&test.SampleRate, 4, 1, file);
		fread(&test.ByteRate, 4, 1, file);
		fread(&test.BlockAlign, 2, 1, file);
		fread(&test.BitsPerSample, 2, 1, file);
		fread(&test.DumpedChunkID, 1, 4, file);
		fread(&test.DumpedChunkSize, 4, 1, file);
		unsigned char DumpText[4000];
		fread(DumpText, 1, test.DumpedChunkSize, file);
		fread(test.SubChunk2ID, 1, 4, file);
		fread(&test.SubChunk2Size, 4, 1, file);
		printf("\n\n==========WAV INFO==========\n");
		ShowInfoWav(test);

	}
	else {
		printf("\nProblem occured during file loading. Exiting program...\n");
	}
	return file;
}

