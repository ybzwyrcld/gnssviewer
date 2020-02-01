// Copyright(c) 2019, 2020
// Yuming Meng <mengyuming@hotmail.com>.
// All rights reserved.
//
// Author:  Yuming Meng
// Date:  2020-01-16 15:02
// Description:  No.

#include "gnss_viewer.h"

#include <unistd.h>
#include <time.h>

#include <chrono>
#include <thread>
#include <fstream>

#include "websocket/websocket.h"


using libnmeaparser::GPSLocation;
using libwebsocket::WebSocket;

namespace gnssviewer {

namespace {

int StringSplit(const std::string &str, const std::string &div,
                std::vector<std::string> *out) {
  if (out == nullptr) return -1;
  out->clear();
  std::string::size_type pos1 = 0;
  std::string::size_type pos2 = str.find(div);
  while (std::string::npos != pos2) {
    out->push_back(str.substr(pos1, pos2 - pos1));
    pos1 = pos2 + div.size();
    pos2 = str.find(div, pos1);
  }
  if (pos1 != str.length()) out->push_back(str.substr(pos1));
  return 0;
}

}  // namespace

GNSSViewer::GNSSViewer() {}

GNSSViewer::~GNSSViewer() {}

int GNSSViewer::Init(const std::string &ip, const int &port,
                     const std::string &path) {
  if (access(path.c_str(), F_OK) < 0) return -1;
  server_ip_ = ip;
  server_port_ = port;
  nmea_file_path_ = path;
  nmea_lines_.clear();
  return 0;
}

bool GNSSViewer::Run(const int &time_out) {
  std::ifstream ifs;
  std::string line;
  ifs.open(nmea_file_path_, std::ios::in);
  if (!ifs.is_open()) {
    printf("Open file failed!!!\n");
    return false;
  }
  while (getline(ifs, line)) {
    nmea_lines_.push_back(line);
  }
  parser_.Init();
  server_.Init(server_ip_, server_port_, 10);  // Set ip="": listen any address.
  std::thread thread = std::thread(&GNSSViewer::ThreadHandler, this, time_out);
  thread.detach();
  printf("Service is running!\n");
  return true;
}

void GNSSViewer::ThreadHandler(const int &time_out) {
  std::string result;
  WebSocket websocket;
  websocket.set_fin(1);
  websocket.set_opcode(1);
  websocket.set_mask(0);
  std::vector<char> msg;
  std::vector<char> msgout;
  // Set the callback function when receiving data.
  server_.OnReceived(
      [&](const int &fd, const char *buffer, const int &size) {});
  server_.Run(3000);  // 3000 ms epoll timeout.
  parser_.OnLocationCallback([&](const GPSLocation &location) {
#if 0
    printf(
        "time: %s, status: %d, age: %f, accuracy: %f, "
        "log: %lf%c, lat: %lf%c, alt: %f, su: %d, "
        "speed: %f, bearing: %f\r\n",
        location.utc_time, location.position, location.age, location.accuracy,
        location.longitude, location.longitude_hemisphere, location.latitude,
        location.latitude_hemisphere, location.altitude,
        location.satellites_used, location.speed, location.bearing);
#endif
    result = "longitude:" + std::to_string(location.longitude);
    result += " latitude:" + std::to_string(location.latitude);
    result.push_back('\0');
    printf("send{%lu]: %s\n", result.size(), result.c_str());
    msg.clear();
    msg.assign(result.begin(), result.end());
    websocket.FormDataGenerate(msg, &msgout);
    server_.SendToAll(msgout.data(), msgout.size());
  });
  auto start_time = time(NULL);
  auto stop_time = start_time;
  uint32_t pos = 0;
  uint32_t lines = nmea_lines_.size();
  // printf("%d\n", lines);
  // TODO(mengyuming@hotmail.com): Only parse GGA and RMC for now.
  parse_lines_ = 2;
  while (1) {
    if ((stop_time = time(NULL)) - start_time >= 1) {
      for (int i = 0; i < parse_lines_; ++i) {
        parser_.PutNmeaLine(nmea_lines_[(pos++) % lines]);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      start_time = stop_time = time(NULL);
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

}  // namespace gnssviewer
