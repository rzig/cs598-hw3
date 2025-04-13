#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <vector>

float tdiff(struct timeval *start, struct timeval *end) {
  return (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
}

struct alignas(64) Planet {
  double mass;
  double x;
  double y;
  double vx;
  double vy;
};

unsigned long long seed = 100;

unsigned long long randomU64() {
  seed ^= (seed << 21);
  seed ^= (seed >> 35);
  seed ^= (seed << 4);
  return seed;
}

double randomDouble() {
  unsigned long long next = randomU64();
  next >>= (64 - 26);
  unsigned long long next2 = randomU64();
  next2 >>= (64 - 26);
  return ((next << 27) + next2) / (double)(1LL << 53);
}

int nplanets;
int timesteps;
double dt;
double G;

struct PlanetMaintentance {
  std::vector<Planet> current;
  std::vector<Planet> next;
};

void next(PlanetMaintentance &planetsM) {
  auto &planets = planetsM.current;
  auto &nextplanets = planetsM.next;
  // Planet *nextplanets =
  //     (Planet *)std::aligned_alloc(alignof(Planet), sizeof(Planet) *
  //     nplanets);
  for (int i = 0; i < nplanets; i++) {
    nextplanets[i].vx = planets[i].vx;
    nextplanets[i].vy = planets[i].vy;
    nextplanets[i].mass = planets[i].mass;
    nextplanets[i].x = planets[i].x;
    nextplanets[i].y = planets[i].y;
  }

  for (alignas(64) int i = 0; i < nplanets; i++) {
    // nextplanets[i].vx = planets[i].vx;
    // nextplanets[i].vy = planets[i].vy;
    // nextplanets[i].mass = planets[i].mass;
    // nextplanets[i].x = planets[i].x;
    // nextplanets[i].y = planets[i].y;
    for (alignas(64) int j = 0; j < nplanets; j++) {
      double dx = planets[j].x - planets[i].x;
      double dy = planets[j].y - planets[i].y;
      double distSqr = dx * dx + dy * dy + 0.0001;
      double invDist = planets[i].mass * planets[j].mass / sqrt(distSqr);
      double invDist3 = invDist * invDist * invDist;
      nextplanets[i].x += dx * invDist3;
      nextplanets[i].y += dy * invDist3;
    }
  }

  for (alignas(64) int i = 0; i < nplanets; i++) {
    nextplanets[i].x += dt * nextplanets[i].vx;
    nextplanets[i].y += dt * nextplanets[i].vy;
  }
  // free(planets);
  // return nextplanets;
}

int main(int argc, const char **argv) {
  if (argc < 2) {
    printf("Usage: %s <nplanets> <timesteps>\n", argv[0]);
    return 1;
  }
  nplanets = atoi(argv[1]);
  timesteps = atoi(argv[2]);
  dt = 0.1;
  G = 6.6743;

  // std::vector<Planet> planets (nplanets);
  // std::vector<Planet> nextplanets (nplanets);

  PlanetMaintentance pm{.current = std::vector<Planet>(nplanets),
                        .next = std::vector<Planet>(nplanets)};

  auto &planets = pm.current;

  for (int i = 0; i < nplanets; i++) {
    planets[i].mass = randomDouble() + 0.1;
    planets[i].x = randomDouble() * 100 - 50;
    planets[i].y = randomDouble() * 100 - 50;
    planets[i].vx = randomDouble() * 5 - 2.5;
    planets[i].vy = randomDouble() * 5 - 2.5;
  }

  struct timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0; i < timesteps; i++) {
    next(pm);
    std::swap(pm.current, pm.next);
    // Planet *tmp = pm.current;
    // pm.current = pm.next;
    // pm.next = tmp;
    // std::swap(pm.next, pm.current);
    // planets = next(planets);
    // printf("x=%f y=%f\n", planets[nplanets-1].x, planets[nplanets-1].y);
  }
  gettimeofday(&end, NULL);
  printf("Total time to run simulation %0.6f seconds, final location %f %f\n",
         tdiff(&start, &end), planets[nplanets - 1].x, planets[nplanets - 1].y);

  return 0;
}
