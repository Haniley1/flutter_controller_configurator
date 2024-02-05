import 'package:flutter/material.dart';
import 'package:flutter_controller_configurator/pages/AddDevicePage.dart';
import 'package:flutter_controller_configurator/storage/DeviceStorage.dart';
import 'package:flutter_controller_configurator/widgets/device-list/DeviceListView.dart';
import 'package:flutter_controller_configurator/widgets/Layout.dart';
import 'package:provider/provider.dart';
import 'package:upnp2/upnp.dart' hide Icon;

import '../storage/SSDPManager.dart';
import '../storage/SSDPStorage.dart';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<StatefulWidget> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  List<Device> ssdpList = [];
  List<Device> savedDevicesList = [];
  final DeviceStorage deviceStorage = DeviceStorage();
  late SSDPStorage ssdpStorage;
  late SSDPManager manager;

  void onDeviceFound(Device device) async {
    ssdpStorage.addDevice(device);
    final bool isDeviceSaved = await deviceStorage.getDeviceName(device.uuid!) != null;

    if (isDeviceSaved) {
      savedDevicesList.add(device);
    }
  }

  void onReloadPressed() {
    ssdpStorage.clearDevices();
    savedDevicesList.clear();
    manager.discoverDevices();
  }

  @override
  void initState() {
    super.initState();

    WidgetsBinding.instance.addPostFrameCallback((timeStamp) => manager.discoverDevices(),);
  }

  @override
  Widget build(BuildContext context) {
    manager = SSDPManager(onDeviceFound: onDeviceFound);
    ssdpStorage = Provider.of<SSDPStorage>(context);
    ssdpList = ssdpStorage.getDevices();

    void openAddDevicePage() {
      Navigator.push(context, MaterialPageRoute(builder: (context) => const AddDevicePage()));
    }

    return PageLayout(
      title: "Список устройств",
      bodyWidget: DeviceListView(devices: savedDevicesList),
      onReloadPressed: onReloadPressed,
      showFloating: true,
      onFloatingPressed: openAddDevicePage,
    );
  }

}
