import 'package:flutter/material.dart';
import 'package:flutter_controller_configurator/storage/SSDPStorage.dart';
import 'package:flutter_controller_configurator/widgets/Layout.dart';
import 'package:flutter_controller_configurator/widgets/SSPDListView.dart';
import 'package:provider/provider.dart';
import '../storage/SSDPManager.dart';

import '../widgets/device-list/DeviceListView.dart';

class AddDevicePage extends StatefulWidget {
  const AddDevicePage({ super.key });

  @override
  State<AddDevicePage> createState() => _AddDevicePageState();
}

class _AddDevicePageState extends State<AddDevicePage> {
  @override
  Widget build(BuildContext context) {
    final ssdpStorage = Provider.of<SSDPStorage>(context);
    final ssdpList = ssdpStorage.getDevices();
    final manager = SSDPManager(onDeviceFound: (device) => ssdpStorage.addDevice(device));

    void onReloadPressed() {
      ssdpStorage.clearDevices();
      manager.discoverDevices();
    }

    return PageLayout(
      title: "Добавление устройства",
      bodyWidget: SSPDListView(devices: ssdpList),
      onReloadPressed: onReloadPressed,
    );
  }

}
