import 'dart:io';

import 'package:flutter/material.dart';
import 'package:upnp2/upnp.dart';

class SSDPStorage with ChangeNotifier {
  Map<String, Device> devices = {};

  void addDevice(Device device) {
    final bool hasDevice = devices.containsKey(device.uuid);

    if (!hasDevice) {
      devices[device.uuid!] = device;
    }

    notifyListeners();
  }

  void clearDevices() {
    devices.clear();
    notifyListeners();
  }

  List<Device> getDevices() {
    return devices.values.map((device) => device).toList();
  }
}
