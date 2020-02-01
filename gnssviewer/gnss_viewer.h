// Copyright(c) 2019, 2020
// Yuming Meng <mengyuming@hotmail.com>.
// All rights reserved.
//
// Author:  Yuming Meng
// Date:  2020-01-16 15:02
// Description:  No.

#ifndef GNSSVIEWER_GNSS_VIEWER_H_
#define GNSSVIEWER_GNSS_VIEWER_H_

#include <string>
#include <vector>

#include "websocket/server.h"
#include "nmea_parser.h"


namespace gnssviewer {

class GNSSViewer {
 public:
  GNSSViewer();
  ~GNSSViewer();
  int Init(const std::string &ip, const int &port, const std::string &path);
  bool Run(const int &timeout);

 private:
  void ThreadHandler(const int &time_out);

  std::string server_ip_;
  int server_port_ = 0;
  int parse_lines_ = 0;
  std::string nmea_file_path_;
  libwebsocket::Server server_;
  libnmeaparser::NmeaParser parser_;
  std::vector<std::string> nmea_lines_;
};

}  // namespace gnssviewer

#endif  // GNSSVIEWER_GNSS_VIEWER_H_
