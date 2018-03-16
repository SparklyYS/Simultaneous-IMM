#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define INFTY int(1 << 30)
#define LL_DIGIT 64
using namespace std;

typedef long vertex_t;
typedef long index_t;
typedef long long vertex_tt;

inline off_t fsize(const char *filename)
{
	struct stat st;
	if (stat(filename, &st) == 0)
		return st.st_size;
	return -1;
}

int main(int argc, char **argv)
{
	int fd, i;
	char *ss_head;

	cout << "Input: ./exe tuplefile(text)" << endl;
	if (argc < 2)
	{
		cout << "Wrong Input" << endl;
		exit(-1);
	}
	size_t file_size = fsize(argv[1]);

	fd = open(argv[1], O_CREAT | O_RDWR, 00666);
	if (fd == -1)
	{
		printf("%s open error", argv[1]);
		perror("open");
		exit(-1);
	}

	ss_head = (char *)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	assert(ss_head != MAP_FAILED);

	size_t curr = 0;
	size_t next = 0;

	cout << "step 1...." << endl;
	// step 1 vertex_count, edge_count
	size_t edge_count = 0;
	size_t vertex_count = 0;
	vertex_t v_max = 0;
	vertex_t v_min = INFTY;
	vertex_t a;

	while (next < file_size)
	{
		char *sss = ss_head + curr;
		a = atol(sss);
		if (v_max < a)
		{
			v_max = a;
		}
		if (v_min > a)
		{
			v_min = a;
		}

		while ((ss_head[next] != ' ') && (ss_head[next] != '\n') && (ss_head[next] != '\t'))
			next++;
		while ((ss_head[next] == ' ') || (ss_head[next] == '\n') || (ss_head[next] == '\t'))
			next++;
		curr = next;

		edge_count++;
	}

	edge_count >>= 1;
	const index_t line_count = edge_count;

	vertex_count = v_max - v_min + 1;
	assert(v_min < INFTY);

	// print step 1 information
	cout << "edge count: " << edge_count << endl;
	cout << "max vertex id: " << v_max << endl;
	cout << "min vertex id: " << v_min << endl;
	cout << "vertex count: " << vertex_count << endl;

	// step 2 each file size
	// csr_file
	char filename[256];
	sprintf(filename, "%s_csr.bin", argv[1]);
	int fd1 = open(filename, O_CREAT | O_RDWR, 00666);
	assert(ftruncate(fd1, edge_count * sizeof(vertex_t)) == 0);
	vertex_t *adj = (vertex_t *)mmap(NULL, edge_count * sizeof(vertex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
	assert(adj != MAP_FAILED);

	// beg_file
	sprintf(filename, "%s_beg.bin", argv[1]);
	int fd2 = open(filename, O_CREAT | O_RDWR, 00666);
	assert(ftruncate(fd2, (vertex_count + 1) * sizeof(vertex_t)) == 0);
	vertex_t *beg = (vertex_t *)mmap(NULL, (vertex_count + 1) * sizeof(vertex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
	assert(beg != MAP_FAILED);

	// deg_file
	sprintf(filename, "%s_deg.bin", argv[1]);
	int fd3 = open(filename, O_CREAT | O_RDWR, 00666);
	assert(ftruncate(fd3, (vertex_count) * sizeof(index_t)) == 0);
	index_t *deg = (index_t *)mmap(NULL, vertex_count * sizeof(index_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd3, 0);
	assert(deg != MAP_FAILED);

	// weight_file
	sprintf(filename, "%s_weight.bin", argv[1]);
	int fd4 = open(filename, O_CREAT | O_RDWR, 00666);
	assert(ftruncate(fd4, (edge_count) * (sizeof(vertex_tt) * 2)) == 0);
	vertex_tt *weight = (vertex_tt *)mmap(NULL, edge_count * (sizeof(vertex_tt) * 2), PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);
	assert(weight != MAP_FAILED);

	// head_file
	sprintf(filename, "%s_head.bin", argv[1]);
	int fd5 = open(filename, O_CREAT | O_RDWR, 00666);
	assert(ftruncate(fd5, (edge_count) * sizeof(vertex_t)) == 0);
	vertex_t *head = (vertex_t *)mmap(NULL, edge_count * sizeof(vertex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd5, 0);
	assert(head != MAP_FAILED);

	//step 3 write degree
	for (i = 0; i < vertex_count; i++)
	{
		deg[i] = 0;
	}

	curr = 0;
	next = 0;
	vertex_t src, dest;
	size_t offset = 0;
	while (offset < line_count)
	{
		char *sss = ss_head + curr;
		src = atol(sss) - v_min;

		while ((ss_head[next] != ' ') && (ss_head[next] != '\n') && (ss_head[next] != '\t'))
			next++;
		while ((ss_head[next] == ' ') || (ss_head[next] == '\n') && (ss_head[next] == '\t'))
			next++;

		curr = next;

		sss = ss_head + curr;
		dest = atol(sss) - v_min;
		while ((ss_head[next] != ' ') && (ss_head[next] != '\n') && (ss_head[next] != '\t'))
			next++;
		while ((ss_head[next] == ' ') || (ss_head[next] == '\n') && (ss_head[next] == '\t'))
			next++;

		curr = next;

		deg[src]++;
		offset++;
	}

	// step 4 construct beg
	beg[0] = 0;
	beg[vertex_count] = edge_count;
	cout << "Construct beg ... " << endl;
	for (i = 1; i < vertex_count; i++)
	{
		beg[i] = beg[i - 1] + deg[i - 1];
	}

	// step 5 construct csr
	cout << "Construct CSR ... " << endl;
	offset = 0;
	curr = 0;
	next = 0;

	// initial the index
	size_t *index = (size_t *)malloc(vertex_count * sizeof(size_t));
	for (i = 0; i < vertex_count; i++)
	{
		index[i] = 0;
	}

	//cout << "log..." << endl;
	while (offset < line_count)
	{
		char *sss = ss_head + curr;
		src = atol(sss) - v_min;

		while ((ss_head[next] != ' ') && (ss_head[next] != '\n') && (ss_head[next] != '\t'))
			next++;
		while ((ss_head[next] == ' ') || (ss_head[next] == '\n') && (ss_head[next] == '\t'))
			next++;

		curr = next;
		sss = ss_head + curr;
		dest = atol(sss) - v_min;

		// write adj_vert
		adj[beg[src] + index[src]] = dest;
		head[beg[src] + index[src]] = src;

		while ((ss_head[next] != ' ') && (ss_head[next] != '\n') && (ss_head[next] != '\t'))
			next++;
		while ((ss_head[next] == ' ') || (ss_head[next] == '\n') && (ss_head[next] == '\t'))
			next++;

		curr = next;
		index[src]++;
		offset++;
	}
	//cout << "log..." << endl;
	free(index);

	// step 5 write the edge_stat
	for (i = 0; i < edge_count; i++)
	{
		index_t idx = i << 1;
		double prob = 1.0 / deg[head[i]];

		srand((unsigned)time(NULL));
		//generate edge_stat
		for (int j = 0; j < LL_DIGIT; j++)
		{
			double randDouble = rand() / (RAND_MAX + 1.0);
			if (randDouble < prob)
			{
				weight[idx] |= (1 << j);
			}
			randDouble = rand() / (RAND_MAX + 1.0);
			if (randDouble < prob)
			{
				weight[idx + 1] |= (1 << j);
			}
		}
	}

	munmap(ss_head, sizeof(char) * file_size);
	munmap(adj, sizeof(vertex_t) * edge_count);
	munmap(beg, sizeof(vertex_t) * (vertex_count + 1));
	munmap(deg, sizeof(index_t) * vertex_count);
	munmap(weight, sizeof(vertex_tt) * 2 * edge_count);
	munmap(head, sizeof(vertex_t) * edge_count);

	close(fd);
	close(fd1);
	close(fd2);
	close(fd3);
	close(fd4);
	close(fd5);

	return 0;
}
