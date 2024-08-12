## Конвертер изображений

Библиотека позволяет конвертировать изображения 3-х форматов (jpeg, bmp, ppm). Также, производить преобразования: вертикальное и горизонтальное отражение, негатив, преобразование Собеля (контур).

## Применение

- Запустить готовый файл ```imgconv.exe```, который расположен в директории ```\ImgConverter\build\Release```
- Поместить изображение в директорию, где расположен исполняемый файл
- Следовать указаниям в программе. Пример работы:

<img src="https://raw.githubusercontent.com/VladimirKMr/Image_Converter-JPG-BMP-PPM/main/screens/main.png"/>

## Сборка с применением Cmake и Visual Studio

- Библиотека под Win x64 собрана и расположена по пути ```\libjpeg\```
- Корректируем путь до библиотеки в ```\ImgLib\CMakeLists.txt```
- Корректируем ```\ImgConverter\CMakeLists.txt``` если требуется
- Создаем\очищаем директорию ```ImgConverter\build``` и переходим в неё в терминале
- Пример команды для сборки VS: ```cmake ../ -G "Visual Studio 17 2022"```
- В директории ```ImgConverter\build``` появится файл проекта VS ```.sln``` - запускаем и собираем проект в VS.