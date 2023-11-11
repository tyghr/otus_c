Разбор логов HTTP-сервера

Сложность
★★★☆☆

Цель:
- Получить опыт создания многопоточных приложений.

Описание/Пошаговая инструкция выполнения домашнего задания:
- Написать программу, в заданное число потоков
- разбирающую логи веб-сервера в стандартном ("combined") формате
- и подсчитывающую агрегированную статистику:
- общее количество отданных байт,
- 10 самых "тяжёлых" по суммарному трафику URL'ов
- и 10 наиболее часто встречающихся Referer'ов.


Требования:
- Создано консольное приложение, принимающее аргументами командной строки директорию с логами и количество потоков.
- Приложение корректно подсчитывает требуемые статистические данные (файлы логов для проверки прилагаются).
- Приложение корректно обрабатывает случай пустой директории с логами.
- Приложение корректно обрабатывает ошибки доступа к файлам.
- Код компилируется без предупреждений с ключами компилятора -Wall -Wextra -Wpedantic -std=c11.

Вспомогательные материалы
- [Описание “combined” формата логов](http://httpd.apache.org/docs/2.4/logs.html#combined)

----------
### Решение

Для синхронизации между потоками была использована библиотека
- [Библиотека chan (Pure C implementation of Go channels)](https://github.com/tylertreat/chan)

```shell
$ make
$ ./app -d /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc -t 1

processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.2
processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.3
processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.4
processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.5
processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.6
processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.7
processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.8
processing /mnt/c/Users/tyghr/Downloads/19_threads_homework-12926-1514cc/access.log.9
sent all jobs
All total bytes: 279204625538

Top refs by times:
        times: 9334718  ref: "-"
        times: 1200038  ref: "https://www.google.com/"
        times: 414808   ref: "https://yandex.ru/"
        times: 246342   ref: "https://www.google.ru/"
        times: 194706   ref: "https://baneks.site/best/"
        times: 100254   ref: "https://baneks.site/new/"
        times: 84240    ref: "http://yandex.ru/searchapp?text="
        times: 74820    ref: "https://baneks.site/"
        times: 64942    ref: "https://baneks.site/best/?p=2"
        times: 49456    ref: "https://www.google.com.ua/"

Top urls by size:
        total_size: 42504752962 url: /sitemap.xml
        total_size: 2133096622  url: /best/
        total_size: 1776085712  url: /
        total_size: 1153080316  url: /new/
        total_size: 1078666002  url: /%D1%81%D1%82%D0%BE%D1%8F%D1%82-%D0%BF%D0%B0%D1%81%D1%81%D0%B0%D0%B6%D0%B8%D1%80%D1%8B-%D0%B2-%D0%B0%D1%8D%D1%80%D0%BE%D0%BF%D0%BE%D1%80%D1%82%D1%83-%D0%BF%D0%BE%D1%81%D0%B0%D0%B4%D0%BE%D1%87%D0%BD%D1%8B%D0%B9-%D0%B4%D0%BE%D1%81%D0%BC%D0%BE%D1%82%D1%80/?p=4
        total_size: 853984744   url: /%D0%B4%D0%B0-%D0%B0%D0%BB%D1%91-%D0%B4%D0%B0-%D0%B4%D0%B0-%D0%BD%D1%83-%D0%BA%D0%B0%D0%BA-%D1%82%D0%B0%D0%BC-%D1%81-%D0%B4%D0%B5%D0%BD%D1%8C%D0%B3%D0%B0%D0%BC%D0%B8/
        total_size: 763750570   url: /%D0%B6%D0%B5%D0%BD%D1%89%D0%B8%D0%BD%D0%B0-%D0%B8-%D0%B5%D1%91-%D0%BB%D1%8E%D0%B1%D0%BE%D0%B2%D0%BD%D0%B8%D0%BA-%D0%BD%D0%B0%D1%85%D0%BE%D0%B4%D0%B8%D0%BB%D0%B8%D1%81%D1%8C-%D0%B2-%D0%B4%D0%BE%D0%BC%D0%B5-%D0%BF%D0%BE%D0%BA%D0%B0-%D0%BC%D1%83%D0%B6/
        total_size: 648695810   url: /%D0%B2%D1%81%D1%82%D1%80%D0%B5%D1%87%D0%B0%D0%BB%D0%B8%D1%81%D1%8C-%D1%81-%D0%B4%D0%B5%D0%B2%D1%83%D1%88%D0%BA%D0%BE%D0%B9-%D1%83%D0%B6%D0%B5-2-%D0%B3%D0%BE%D0%B4%D0%B0-%D0%B1%D1%8B%D0%BB-%D1%81%D0%B5%D0%BA%D1%81-%D0%B8/
        total_size: 464625796   url: /%D0%BA%D1%80%D1%83%D1%82%D0%BE%D0%B9-%D0%B4%D0%B6%D0%B0%D0%B7%D0%BE%D0%B2%D1%8B%D0%B9-%D0%B1%D0%B0%D1%80-%D0%BF%D1%80%D0%BE%D0%B2%D0%BE%D0%B4%D0%B8%D1%82-%D0%BA%D0%BE%D0%BD%D0%BA%D1%83%D1%80%D1%81/
        total_size: 458865158   url: /-%D0%B4%D0%B5%D0%B2%D1%83%D1%88%D0%BA%D0%B0-%D0%B2%D0%B0%D1%88%D0%B8-%D1%80%D0%BE%D0%B4%D0%B8%D1%82%D0%B5%D0%BB%D0%B8-%D1%81%D0%BB%D1%83%D1%87%D0%B0%D0%B9%D0%BD%D0%BE-%D0%BD%D0%B5-%D0%BE%D1%85%D0%BE%D1%82%D0%BD%D0%B8%D0%BA%D0%B8-%D0%BD%D0%B5%D1%82/
```