import 'package:flutter/material.dart';
import 'package:flutter_controller_configurator/pages/HomePage.dart';
import 'package:flutter_controller_configurator/storage/DeviceStorage.dart';
import 'package:flutter_controller_configurator/widgets/Layout.dart';
import 'package:upnp2/upnp.dart' hide Icon;

class DeviceForm extends StatefulWidget {
  const DeviceForm({ super.key, required this.ssdpDevice, this.title, this.minThreshold, this.maxThreshold });

  final Device ssdpDevice;
  final String? title;
  final String? minThreshold;
  final String? maxThreshold;

  @override
  State<DeviceForm> createState() => _DeviceFormState();
}

class _DeviceFormState extends State<DeviceForm> {
  final _formKey = GlobalKey<FormState>();
  final deviceStorage = DeviceStorage();
  final _deviceNameController = TextEditingController();
  final _minThresholdController = TextEditingController();
  final _maxThresholdController = TextEditingController();
  DateTime _selectedDate = DateTime.now();

  Future<void> _selectDate(BuildContext context) async {
    final DateTime? picked = await showDatePicker(
      context: context,
      initialDate: _selectedDate,
      firstDate: DateTime(2000),
      lastDate: DateTime(2101),
    );

    if (picked != null && picked != _selectedDate) {
      setState(() {
        _selectedDate = picked;
      });
    }
  }

  void onSubmit() {
    final bool? formValid = _formKey.currentState?.validate();

    if (formValid != null && formValid) {
      // Process the form data
      print('UUID: ${widget.ssdpDevice.uuid}');
      print('Device Name: ${_deviceNameController.text}');
      print('Minimum Threshold: ${_minThresholdController.text}');
      print('Maximum Threshold: ${_maxThresholdController.text}');
      print('Date Checked: $_selectedDate');

      deviceStorage.addDevice(widget.ssdpDevice.uuid!, _deviceNameController.text);
      Navigator.push(context, MaterialPageRoute(builder: (context) => const HomePage()));
    }
  }

  @override
  Widget build(BuildContext context) {
    return PageLayout(
      title: "Добавление устройства",
      bodyWidget: Material(
        child: Form(
          key: _formKey,
          child: Padding(
            padding: const EdgeInsets.all(20.0),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: <Widget>[
                TextFormField(
                  controller: _deviceNameController..text = widget.title ?? '',
                  decoration: InputDecoration(labelText: 'Device Name'),
                  validator: (value) {
                    if (value == "") {
                      return 'Please enter device name';
                    }
                    return null;
                  },
                ),
                TextFormField(
                  controller: _minThresholdController..text = widget.minThreshold ?? '',
                  decoration: InputDecoration(labelText: 'Minimum Threshold'),
                  keyboardType: TextInputType.number,
                  validator: (value) {
                    if (value == "") {
                      return 'Please enter minimum threshold';
                    }
                    return null;
                  },
                ),
                TextFormField(
                  controller: _maxThresholdController..text = widget.maxThreshold ?? '',
                  decoration: InputDecoration(labelText: 'Maximum Threshold'),
                  keyboardType: TextInputType.number,
                  validator: (value) {
                    if (value == "") {
                      return 'Please enter maximum threshold';
                    }
                    return null;
                  },
                ),
                SizedBox(height: 20),
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: <Widget>[
                    Text('Date Checked: ${_selectedDate.month}/${_selectedDate.day}/${_selectedDate.year}'),
                    IconButton(
                      icon: Icon(Icons.calendar_today),
                      onPressed: () => _selectDate(context),
                    ),
                  ],
                ),
                SizedBox(height: 20),
                ElevatedButton(
                  onPressed: onSubmit,
                  child: Text('Submit'),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
