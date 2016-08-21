#include <toolDefines.h>

IN(10)
OUT(10)
POINTS(1) = {{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}, {11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0}}};
SEEDS(1) = {{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}, {11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0}}};

void func(NUMBER* x, NUMBER* y) {
  int world_rank;
  medi::AMPI_Comm_rank(AMPI_COMM_WORLD, &world_rank);
  int world_size;
  medi::AMPI_Comm_size(AMPI_COMM_WORLD, &world_size);

  size_t size = sizeof(char) * 1024 * 1024;
  void* buf = malloc(size);
  medi::AMPI_Buffer_attach(buf, size);

  medi::AMPI_Request request;
  if(world_rank == 0) {
    medi::AMPI_Ibsend(x, 10, mpiNumberType, 1, 42, AMPI_COMM_WORLD, &request);
  } else {
    medi::AMPI_Irecv(y, 10, mpiNumberType, 0, 42, AMPI_COMM_WORLD, &request);
  }

  medi::AMPI_Wait(&request, AMPI_STATUS_IGNORE);
}
