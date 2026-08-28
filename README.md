# 🎓 ITMO C++ Course — Lab Projects

> Репозиторий с лабораторными работами по курсу **«Программирование на C++»**, ИТМО, 2025–2026 учебный год.

![C++](https://img.shields.io/badge/C%2B%2B-23-blue?logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.12+-064F8C?logo=cmake)
![Google Test](https://img.shields.io/badge/Tests-Google%20Test-4285F4?logo=google)
![CI](https://img.shields.io/badge/CI-GitHub%20Actions-2088FF?logo=githubactions)

---

## 📋 Обзор проектов

| # | Проект | Краткое описание | Ключевые темы |
|:-:|--------|-----------------|---------------|
| 01 | [Template Engine](#01--template-engine) | Движок шаблонов с подстановкой переменных | Сырые указатели, C-строки, работа с файлами |
| 02 | [int2025_t](#02--int2025_t) | Целое знаковое число на 2025 бит | Представление чисел, арифметика указателей, структуры |
| 03 | [ArgParser](#03--argparser) | Библиотека парсинга аргументов CLI | Динамическая память, перегрузка функций, namespace |
| 04 | [HamArc](#04--hamarc) | Помехоустойчивый архиватор (коды Хэмминга) | ООП, классы, перегрузка операторов, этапы компиляции |
| 06 | [Circular Buffer](#06--circular-buffer) | STL-совместимый кольцевой буфер | Шаблоны, итераторы, аллокаторы, STL-контейнеры |
| 07 | [Routes Finder](#07--routes-finder) | Поиск маршрутов через Yandex API | HTTP-запросы, JSON, кэширование, внешние библиотеки |
| 08 | [Adapters Lib](#08--adapters-lib) | Библиотека ленивых адаптеров для потоков данных | Лямбды, type erasure, `std::function`, ленивые вычисления |
| 09 | [Task Scheduler](#09--task-scheduler) | DAG-планировщик задач + приложение-рекомендатор | Move semantics, variadic templates, perfect forwarding |
| 10 | [In-Memory Database](#10--in-memory-database) | Key-value БД, совместимая с Valkey/Redis | `std::variant`/`std::visit`, concepts, SFINAE, pattern matching |

---

## 🔧 Стек технологий

- **Язык:** C++23
- **Система сборки:** CMake 3.12+
- **Тестирование:** Google Test / Google Mock
- **CI/CD:** GitHub Actions
- **Внешние библиотеки:** [nlohmann/json](https://github.com/nlohmann/json), [C++ Requests (cpr)](https://github.com/libcpr/cpr)
- **API:** Yandex Расписания, 2ip, Яндекс Погода, Яндекс Карты

---

## 📂 Подробное описание проектов

### 01 — Template Engine

**Движок шаблонов** — консольная утилита, подставляющая значения переменных в текстовый шаблон.

```
Шаблон:   Hello, {{ user }}! Welcome to {{ university }}.
Данные:   user=Ivan, university=ITMO
Результат: Hello, Ivan! Welcome to ITMO.
```

**Что реализовано:**
- Парсинг аргументов командной строки (`-t`, `-d`, `-o` / `--template=`, `--data=`, `--output=`)
- Чтение и обработка файла данных (формат `key=value`, поддержка комментариев `//` и `#`)
- Подстановка переменных `{{ key }}` в шаблон с динамическим управлением памятью
- Вывод результата в файл или stdout

**Концепции C++:** работа с сырыми указателями и C-строками, `malloc`/`free`, побайтовое чтение файлов, арифметика указателей.

---

### 02 — int2025_t

**Пользовательский тип** для целого знакового числа фиксированной длины — 2025 бит (254 байта). Диапазон значений: $[-2^{2024},~2^{2024} - 1]$.

**Что реализовано:**
- Конвертация из `int32_t` и строки
- Полная арифметика: `+`, `-`, `*`, `/`
- Операторы сравнения `==`, `!=`
- Вывод в поток `operator<<`
- Поведение `wrap around` при переполнении

**Ограничения:** без STL-контейнеров, без `std::bitset` — только массивы и указатели.

**Концепции C++:** представление чисел в памяти, длинная арифметика, массивы, структуры, побитовые операции.

---

### 03 — ArgParser

**Библиотека парсинга аргументов** командной строки. API спроектирован на основе набора тестов (test-driven design).

**Поддерживаемые типы аргументов:**
- Целые числа, вещественные числа
- Флаги (boolean)
- Строки (с ограничением максимальной длины)

**Что реализовано:**
- Полный разбор позиционных и именованных аргументов
- Короткие (`-s`) и длинные (`--sum`) формы
- Поддержка multi-value аргументов
- Динамическое аллоцирование памяти

**Ограничения:** без контейнеров, без шаблонов, без классов.

**Концепции C++:** виртуальное адресное пространство, сегменты памяти, стек вызовов, куча, перегрузка функций, ссылки, namespace.

---

### 04 — HamArc

**Помехоустойчивый архиватор файлов** без сжатия. Использует [коды Хэмминга](https://en.wikipedia.org/wiki/Hamming_code) для обнаружения и исправления ошибок. Формат архива: `.haf` (Hamming Archive File).

**Поддерживаемые операции:**
| Флаг | Действие |
|------|----------|
| `-c, --create` | Создание нового архива |
| `-x, --extract` | Извлечение файлов |
| `-l, --list` | Список файлов в архиве |
| `-a, --append` | Добавление файла |
| `-d, --delete` | Удаление файла |
| `-A, --concatenate` | Слияние двух архивов |

**Модульная архитектура:**
- `hamcode` — кодирование/декодирование по Хэммингу
- `archiveoperation` — операции над архивом
- `filesoperation` — файловый I/O
- `argparser` — парсер CLI (переиспользован из lab 03)

**Концепции C++:** ООП (абстракция, инкапсуляция), специальные методы классов, перегрузка операторов, этапы компиляции, linkage & storage duration.

---

### 06 — Circular Buffer

**STL-совместимый контейнер** — шаблонный кольцевой буфер с поддержкой расширения (`Extendable`).

**Реализованные требования:**
- ✅ [Container](https://en.cppreference.com/w/cpp/named_req/Container)
- ✅ [SequenceContainer](https://en.cppreference.com/w/cpp/named_req/SequenceContainer)
- ✅ [ReversibleContainer](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer)
- ✅ [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer)
- ✅ [RandomAccessIterator](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator)

**Ключевые характеристики:**
- `push_back` / `push_front` / `pop_back` / `pop_front` — O(1)
- `insert` / `erase` — O(N)
- 4 типа итераторов: `iterator`, `const_iterator`, `reverse_iterator`, `const_reverse_iterator`
- Поддержка пользовательских аллокаторов
- Автоматическое расширение ёмкости (×2) в `Extendable`-режиме
- Покрытие тестами ≥ 50% (CI/CD проверка через `gcovr`)

**Концепции C++:** шаблоны классов, аллокаторы (`std::allocator_traits`), итераторы произвольного доступа, RAII, exception safety.

---

### 07 — Routes Finder

**Консольное приложение** для поиска маршрутов из Санкт-Петербурга с не более чем одной пересадкой, используя API Яндекс Расписаний.

**Архитектура:**
```
┌───────────┐     ┌────────────┐     ┌────────────────┐
│ CLI / UI  │────▶│  Request   │────▶│  API Client    │
│           │     │  Handler   │     │  (cpr + JSON)  │
└───────────┘     └────────────┘     └────────────────┘
                        │                     │
                        ▼                     ▼
                  ┌────────────┐     ┌────────────────┐
                  │   Parser   │     │  LRU Cache     │
                  │            │     │  (TTL-based)   │
                  └────────────┘     └────────────────┘
```

**Что реализовано:**
- Интерактивный режим с поддержкой нескольких запросов
- HTTP-запросы через [cpr](https://github.com/libcpr/cpr)
- JSON-парсинг через [nlohmann/json](https://github.com/nlohmann/json)
- LRU-кэширование с ограничением размера и TTL
- Моки сетевых запросов через GMock

**Концепции C++:** исключения, stack unwinding, `std::optional`, `std::expected`, каст-операторы, CRTP.

---

### 08 — Adapters Lib

**Библиотека ленивых адаптеров** для конвейерной обработки данных через оператор `|`, вдохновлённая `std::ranges`.

```cpp
Dir(path, recursive)
    | Filter([](auto& p){ return p.extension() == ".txt"; })
    | OpenFiles()
    | Split("\n ,.")
    | Transform([](auto& s){ /* ... */ return s; })
    | AggregateByKey(0uz, aggregator, key_fn)
    | Out(std::cout);
```

**Реализованные адаптеры (14 шт.):**

| Адаптер | Описание |
|---------|----------|
| `Dir` | Обход директории (рекурсивный) |
| `OpenFiles` | Открытие файловых потоков |
| `Split` | Разбиение по делимитерам |
| `Filter` | Фильтрация по предикату |
| `Transform` | Преобразование элементов |
| `Out` | Вывод в поток |
| `Write` | Запись с разделителем |
| `AsDataFlow` | Контейнер → поток данных |
| `AsVector` | Сбор результатов в вектор |
| `Join` | LEFT JOIN по ключу |
| `DropNullopt` | Фильтрация `std::nullopt` |
| `SplitExpected` | Разделение `std::expected` на два пайплайна |
| `AggregateByKey` | Агрегация по ключу (не ленивая) |
| `KV` / `JoinResult` | Вспомогательные структуры |

**Особенности:**
- Ленивые вычисления — константная память для всех адаптеров (кроме `AggregateByKey` и `Join`)
- Композиция через `operator|`
- Полная совместимость с `range-based for`

**Концепции C++:** лямбды, type erasure, `std::function`, указатели на функции/методы/поля.

---

### 09 — Task Scheduler

**DAG-планировщик задач** — класс `TTaskScheduler` для построения графа вычислений с зависимостями между задачами.

**API:**
```cpp
TTaskScheduler scheduler;

auto task1 = scheduler.add(func1, arg1, arg2);
auto task2 = scheduler.add(func2, task1.getFutureResult<T>());
task2.apply(callback);

scheduler.executeAll();
```

**Что реализовано:**
- `TTaskScheduler::add()` — добавление задачи с произвольными аргументами
- `TTask::getFutureResult<T>()` — получение future-результата (move или ссылка)
- `TTask::apply()` — цепочка обработки результата
- `TTask::getResultSync<T>()` — синхронное получение результата с ленивым вычислением
- Type erasure через `ITaskNode` / `TTaskNode<T>`
- **Places Finder** — приложение-рекомендатор мест отдыха на основе геолокации и погоды (API: 2ip, Яндекс Погода, Яндекс Карты)

**Концепции C++:** value categories, rvalue references, move semantics, perfect forwarding (`std::forward`), variadic templates.

---

### 10 — In-Memory Database

**Локальная key-value in-memory БД**, совместимая с подмножеством команд [Valkey](https://valkey.io/) (Redis-совместимая).

**Поддерживаемые типы данных:**

| Тип | Команды |
|-----|---------|
| **String** | `SET`, `GET`, `STRLEN`, `APPEND`, `EXPIRE`, `TTL` |
| **List** | `LPUSH`, `RPUSH`, `LPOP`, `RPOP`, `LLEN`, `LRANGE`, `LINDEX`, `LSET`, `LINSERT` |
| **Set** | `SADD`, `SREM`, `SISMEMBER`, `SMEMBERS`, `SCARD`, `SUNION`, `SINTER`, `SDIFF`, `SMOVE` |
| **Geo** | `GEOADD`, `GEOPOS`, `GEODIST`, `GEOSEARCH`, `GEOSEARCHSTORE` |
| **Общие** | `TYPE`, `DEL`, `EXISTS`, `KEYS`, `FLUSHDB`, `CONFIG SET/GET`, `DBSIZE`, `MEMORY USAGE` |

**Архитектурные решения:**
- **Command Pattern** — каждая команда = отдельный класс, реализующий интерфейс `Command`
- **Storage** — единое хранилище на `std::variant<string, list, set, GeoIndex>`
- **Управление памятью** — `--maxmemory` с поддержкой суффиксов (`kb`, `mb`, `gb`), OOM-ошибки
- **TTL** — поддержка времени жизни ключей через `std::chrono`
- **Geo** — геопоиск по формуле Хаверсина (R = 6372.8 км)
- **CLI** — интерактивный режим, нечувствительность команд к регистру
- **Паттерн-матчинг ключей** — `KEYS` с glob-паттернами (`*`, `?`)

**Концепции C++:** `std::variant`/`std::visit`, overload pattern, `std::expected`, метафункции, SFINAE, concepts.

---

## 🚀 Сборка и запуск

Каждый проект собирается независимо:

```bash
# Перейти в директорию проекта
cd 06_circular-buffer

# Сборка
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Запуск тестов
cmake --build build --target <test_target>
# Или:
cd build && ctest
```

