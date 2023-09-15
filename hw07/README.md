POSIX

Цель:
- Получить навык поиска утечек памяти с помощью valgrind.

Описание/Пошаговая инструкция выполнения домашнего задания:
- Найти и исправить утечки памяти в [программе](https://github.com/clibs/clib) из приложенного архива.
- Для запуска valgrind следует использовать следующие команды в каталоге с распакованным архивом:
```shell
cd test/package
make valgrind
```

Требования:
- Найдена как минимум одна утечка памяти, создан patch, исправляющий утечку.
- Бонусные баллы за дополнительные найденные и исправленные утечки.

Вспомогательные материалы:
- [Valgrind Memcheck](https://web.stanford.edu/class/archive/cs/cs107/cs107.1218/resources/valgrind.html)
- [Debugging Valgrind Errors](https://student.cs.uwaterloo.ca/~cs241/valgrind/)

---
## Решение:

```diff
diff -ru bkp/clib/deps/rimraf/rimraf.c clib/deps/rimraf/rimraf.c
--- bkp/clib/deps/rimraf/rimraf.c       2023-09-15 19:45:18.693587900 +0300
+++ clib/deps/rimraf/rimraf.c   2023-09-15 20:35:08.066808100 +0300
@@ -36,16 +36,16 @@
         || 0 == strcmp("..", dp->d_name)) continue;
 
     char *f = path_join(path, dp->d_name);
-    if (NULL == f) return -1;
+    if (NULL == f) goto exit_fail;
 
     struct stat s;
-    if (0 != stat(f, &s)) return -1;
+    if (0 != stat(f, &s)) goto exit_fail;
     if (s.st_mode & S_IFDIR) {
       // rimraf dirs
-      if (-1 == rimraf(f)) return -1;
+      if (-1 == rimraf(f)) goto exit_fail;
     } else {
       // unlink files
-      if (-1 == unlink(f)) return -1;
+      if (-1 == unlink(f)) goto exit_fail;
     }
     free(f);
   }
@@ -53,4 +53,9 @@
   closedir(dir);
 
   return rmdir(path);
+
+exit_fail:
+  if (dp) free(dp);
+  closedir(dir);
+  return -1;
 }
diff -ru bkp/clib/deps/wiki-registry/wiki-registry.c clib/deps/wiki-registry/wiki-registry.c
--- bkp/clib/deps/wiki-registry/wiki-registry.c 2023-09-15 19:45:20.470851900 +0300
+++ clib/deps/wiki-registry/wiki-registry.c     2023-09-15 20:26:00.691985800 +0300
@@ -153,11 +153,15 @@
 list_t *
 wiki_registry(const char *url) {
   http_get_response_t *res = http_get(url);
-  if (!res->ok) return NULL;
+  if (!res->ok) goto exit_fail;
 
   list_t *list = wiki_registry_parse(res->data);
   http_get_free(res);
   return list;
+
+exit_fail:
+  if (res) http_get_free(res);
+  return NULL;
 }
 
 /**
diff -ru bkp/clib/src/clib-search.c clib/src/clib-search.c
--- bkp/clib/src/clib-search.c  2023-09-15 19:45:21.054203600 +0300
+++ clib/src/clib-search.c      2023-09-15 20:25:58.534143000 +0300
@@ -106,12 +106,13 @@
 
   debug(&debugger, "setting cache from %s", CLIB_WIKI_URL);
   http_get_response_t *res = http_get(CLIB_WIKI_URL);
-  if (!res->ok)
+  if (!res) return NULL;
+  if (!res->ok) {
+    http_get_free(res);
     return NULL;
+  }
 
   char *html = strdup(res->data);
-  if (NULL == html)
-    return NULL;
   http_get_free(res);
 
   if (NULL == html)
diff -ru bkp/clib/src/common/clib-package.c clib/src/common/clib-package.c
--- bkp/clib/src/common/clib-package.c  2023-09-15 19:45:21.285230400 +0300
+++ clib/src/common/clib-package.c      2023-09-15 20:21:49.870328500 +0300
@@ -663,7 +663,9 @@
 #endif
       json = res->data;
       _debug("status: %d", res->status);
-      if (!res || !res->ok) {
+      if (!res) goto download;
+      if (!res->ok) {
+        http_get_free(res);
         goto download;
       }
       log = "fetch";
```
