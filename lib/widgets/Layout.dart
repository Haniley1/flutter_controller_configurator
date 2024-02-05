import 'package:flutter/material.dart';

class PageLayout extends StatelessWidget {
  const PageLayout({
    super.key,
    required this.bodyWidget,
    required this.title,
    this.onReloadPressed,
    this.showFloating = false,
    this.showReload = false,
    this.onFloatingPressed
  });

  final Widget bodyWidget;
  final String title;
  final bool showFloating;
  final bool showReload;
  final VoidCallback? onReloadPressed;
  final VoidCallback? onFloatingPressed;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text(title),
        actions: [
          IconButton(onPressed: onReloadPressed, icon: const Icon(Icons.refresh))
        ],
      ),
      body: bodyWidget,
      floatingActionButton: showFloating ? FloatingActionButton(onPressed: onFloatingPressed,
        child: const Icon(Icons.add),
      ) : null,
    );
  }
}
