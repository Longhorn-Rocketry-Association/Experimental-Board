#include "mock_imu_mpl.h"
#include <sstream>
#include <vector>

void MockImuWrapper::initialize() {
  flightsim = nullptr;
  gyro_noise = std::vector<NoiseGenerator*>(flightsim::AXES_COUNT, nullptr);
  accel_noise = std::vector<NoiseGenerator*>(flightsim::AXES_COUNT, nullptr);
  mag_noise = std::vector<NoiseGenerator*>(flightsim::AXES_COUNT, nullptr);
}

void MockImuWrapper::setTimestep(float t) {
  timestep = t;
}

void MockImuWrapper::setSimulator(FlightSimulator *sim) {
  flightsim = sim;
}

void MockImuWrapper::setGyroNoise(NoiseGenerator *gen, int axis) {
  flightsim::assert_valid_axis(axis);
  gyro_noise[axis] = gen;
}

void MockImuWrapper::setGyroNoise(NoiseGenerator *gen) {
  gyro_noise[0] = gyro_noise[1] = gyro_noise[2] = gen;
}

void MockImuWrapper::setAccelerationNoise(NoiseGenerator *gen, int axis) {
  flightsim::assert_valid_axis(axis);
  accel_noise[axis] = gen;
}

void MockImuWrapper::setAccelerationNoise(NoiseGenerator *gen) {
  accel_noise[0] = accel_noise[1] = accel_noise[2] = gen;
}

void MockImuWrapper::setMagneticNoise(NoiseGenerator *gen, int axis) {
  flightsim::assert_valid_axis(axis);
  mag_noise[axis] = gen;
}

void MockImuWrapper::setMagneticNoise(NoiseGenerator *gen) {
  mag_noise[0] = mag_noise[1] = mag_noise[2] = gen;
}

void MockImuWrapper::setNoise(NoiseGenerator *gen) {
  setGyroNoise(gen);
  setAccelerationNoise(gen);
  setMagneticNoise(gen);
}

struct ImuData MockImuWrapper::read() const {
  std::vector<float> gyro(flightsim::AXES_COUNT);
  std::vector<float> accel(flightsim::AXES_COUNT);

  // Read from flightsim
  if (flightsim != nullptr) {
    gyro = flightsim->get_rocket_gyro();
    accel = flightsim->get_rocket_acceleration();
  }

  // Apply noise
  for (int i = 0; i < gyro.size(); i++)
    if (gyro_noise[i] != nullptr)
      gyro[i] += gyro_noise[i]->gen(timestep);

  for (int i = 0; i < accel.size(); i++)
    if (accel_noise[i] != nullptr)
      accel[i] += accel_noise[i]->gen(timestep);

  struct ImuData data;
  data.gx = gyro[flightsim::X_AXIS];
  data.gy = gyro[flightsim::Y_AXIS];
  data.gz = gyro[flightsim::Z_AXIS];
  data.ax = accel[flightsim::X_AXIS];
  data.ay = accel[flightsim::Y_AXIS];
  data.az = accel[flightsim::Z_AXIS];

  return data;
}
