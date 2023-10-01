ООП
---
Вариант 1

Сложность
★★★☆☆

Цель:
- Получить навык работы с библиотеками GLib,GTK.

Описание/Пошаговая инструкция выполнения домашнего задания:
- Написать приложение с использованием GTK, отображающее файлы и директории в текущем каталоге.
- Сторонние библиотеки (кроме GLib/GTK) не использовать.

Требования:
- Создано приложение, при запуске рекурсивно отображающее в GtkTreeView-виджете файлы и директории, расположенные в директории, откуда оно было запущено.
- Код компилируется без предупреждений с ключами компилятора -Wall -Wextra -Wpedantic -std=c11. Предупреждения об устаревших API допускаются.

Вспомогательные материалы:
- [Getting Started with GTK](https://docs.gtk.org/gtk4/getting_started.html)
- [Gtk - 4.0: Tree and List Widget Overview](https://docs.gtk.org/gtk4/section-tree-widget.html)
- [File Utilities: GLib Reference Manual](https://developer-old.gnome.org/glib/stable/glib-File-Utilities.html)
---
Вариант 2

Сложность
★★★★★

Цель:
- Получить навык работы с библиотеками GLib, GStreamer.

Описание/Пошаговая инструкция выполнения домашнего задания:
- Написать плагин с элементом для GStreamer, воспроизводящим 16-битные little-endian несжатые моно wav-файлы.
- Сторонние библиотеки (кроме GLib/GStreamer) не использовать.

Требования:
- Создана динамическая библиотека, успешно подгружаемая в пайплайн GStreamer командной приблизительно следующего вида: gst-launch my-wav-element location=test.wav ! audio/x-raw,format=S16LE,channels=1,rate=48000 ! autoaudiosink.
- Тестовый пайплайн с участием созданного модуля успешно проигрывает звуковой файл (файл для проверки прилагается).
- Код компилируется без предупреждений с ключами компилятора -Wall -Wextra -Wpedantic -std=c11.

Вспомогательные материалы:
- [Reading the header of a wav file](https://onestepcode.com/read-wav-header)
- [GStreamer source](https://gitlab.freedesktop.org/gstreamer/gstreamer)
- [gst-element-maker](https://gitlab.freedesktop.org/gstreamer/gst-plugins-bad/-/blob/master/tools/gst-element-maker)
- [Plugin Writer's Guide](https://gstreamer.freedesktop.org/documentation/plugin-development/?gi-language=c)
---
Вариант 1.  
Решение:
```shell
sudo apt install libgtk-4-dev
gcc $(pkg-config --cflags gtk4) -o app main.c $(pkg-config --libs gtk4)
./app
```
---
Вариант 2 (не решено)
1. подготовка
```sh
sudo apt update
sudo apt dist-upgrade
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio

git clone https://gitlab.freedesktop.org/gstreamer/gst-plugins-bad.git
cd gst-plugins-bad/tools
./gst-element-maker wavsrc basesrc
# gstwavsrc.c
# gstwavsrc.h
```
2. реализовать методы:  
__(сначала начал делать этот вариант, но на этом моменте я сдался)__
- create
- is_seekable
- get_size
- start
- set_property
- get_property
3. собрать динамическую библиотеку плагина
    ```gcc -shared -fPIC wavsource.c $(pkg-config --cflags --libs gstreamer-1.0) -o libwavsource.so```
4. запустить отладку
    ```GST_DEBUG=4 GST_PLUGIN_PATH=`pwd` gst-launch-1.0 myplugin location=test.wav ! autoaudiosink```
5. запустить проигрывание с подключенным плагином
    ```gst-launch my-wav-element location=test.wav ! audio/x-raw,format=S16LE,channels=1,rate=48000 ! autoaudiosink```
