import 'package:flutter/material.dart';
import 'package:flutter_controller_configurator/storage/SSDPManager.dart';
import 'package:flutter_controller_configurator/pages/HomePage.dart';
import 'package:flutter_controller_configurator/storage/SSDPStorage.dart';
import 'package:provider/provider.dart';
import 'package:upnp2/upnp.dart';

void main() {
  runApp(MultiProvider(providers: [
    ChangeNotifierProvider(create: (context) => SSDPStorage())
  ], child: const MyApp(),));
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.orange),
        useMaterial3: true,
      ),
      home: const HomePage(),
    );
  }
}
