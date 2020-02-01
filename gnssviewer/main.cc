// Copyright(c) 2019, 2020
// Yuming Meng <mengyuming@hotmail.com>.
// All rights reserved.
//
// Author:  Yuming Meng
// Date:  2020-01-16 15:09
// Description:  No.

#include <chrono>
#include <thread>

#include "gnss_viewer.h"


using gnssviewer::GNSSViewer;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s {nmea-file-path}\n", argv[0]);
    exit(-1);
  }
  GNSSViewer viewer;
  viewer.Init("", 8081, argv[1]);
  viewer.Run(3000);
  while (1) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}
