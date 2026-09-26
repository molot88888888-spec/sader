SADER — Self-describing Discoverable Execution Runtime
Учебный проект по C++20: слой между AI-агентом и набором инструментов (Worker'ов), предоставляющий единый детерминированный протокол для поиска, описания и вызова операций.

Что такое SADER
Представь, что AI-агент хочет что-то сделать: посчитать хеш файла, прочитать CSV, сделать HTTP-запрос. Он не знает заранее, какие инструменты доступны. SADER решает эту задачу тремя командами:

DISCOVER — «какие инструменты умеют X?» (поиск по описанию)

DESCRIBE — «расскажи точно про инструмент Y» (схема аргументов)

CALL — «выполни инструмент Y с аргументами Z» (валидация → выполнение → результат)

Каждый инструмент — это Worker, который сам себя описывает: имя, назначение, схема аргументов, логика выполнения. Executor не знает конкретных Worker'ов — он работает с ними через единый абстрактный интерфейс.

Архитектура
text
AI Agent / User
      │
      │  DISCOVER / DESCRIBE / CALL
      ▼
  CommandParser  ←─ разбирает текст в Command
      │
      │  std::variant<DiscoverCommand, DescribeCommand, CallCommand>
      ▼
  Executor  ←─ мозг: хранит Worker'ов, диспетчеризует
      │
      ├─────── findWorker(name) ───────┐
      ▼                                ▼
  DISCOVER                        DESCRIBE / CALL
  (поиск по описанию)              (схема / выполнение)
      │                                │
      └──────────────┬─────────────────┘
                     ▼
              Worker Registry
              ┌──────┴──────┐
              │             │
        HashWorker      FileWorker  ...  ModelWorker
              │             │                │
              └─────────────┴────────────────┘
                     ▼
                  Result
              (success + value | error)
Ключевые компоненты:

Компонент       Ответственность
CommandParser   Разбирает текст из stdin в структурированный Command
Command std::variant из DiscoverCommand, DescribeCommand, CallCommand
Executor        Хранит vector<unique_ptr<Worker>>, находит Worker'ов по имени, выполняет команды
Worker  Абстрактный интерфейс: name(), description(), schema(), execute()
Result  Структурированный ответ: либо ok(value), либо fail(error)
Schema  Набор ArgumentSpec — контракт Worker'а
Реализованные Worker'ы
#       Worker  Операции        Что демонстрирует
1       HashWorker      md5, sha256     RAII над C-API OpenSSL, кастомный deleter
2       FileWorker      read, size, exists      RAII с ifstream, std::filesystem, error_code
3       JsonWorker      parse, get, type        Внешняя библиотека (nlohmann-json), навигация по пути
4       HttpWorker      get, status     Сетевой ресурс, libcurl, timeout, лимит размера
5       TextWorker      length, words, lines    Обработка строк, switch по операции
6       CsvWorker       count, sum, mean, min, max      Парсинг CSV, агрегация, std::stod с валидацией
7       ProcessWorker   run (whitelist) Запуск процессов через popen, защита от shell-инъекций
8       ModelWorker     normalize, dot, cosine  Векторная математика, проверка размерности
Быстрый старт
Зависимости
C++20 (g++ 13+ или clang++ 16+)

CMake 3.20+

OpenSSL (libssl-dev) — для HashWorker

libcurl (libcurl4-openssl-dev) — для HttpWorker

nlohmann-json (nlohmann-json3-dev) — для JsonWorker

Установка на Ubuntu 24.04:

bash
sudo apt install -y build-essential cmake \
    libssl-dev libcurl4-openssl-dev nlohmann-json3-dev
Сборка
bash
cmake -S . -B build
cmake --build build
Запуск
bash
./build/sader
Примеры
DISCOVER — найти capability
text
SADER> DISCOVER sha256
hash - Calculate md5 and sha256 hashes for text

SADER> DISCOVER file
file - File operations: read content, get size, check existence

SADER> DISCOVER words
text - Text operations: count characters, words, or lines in a string

SADER> DISCOVER checksum
No capabilities found
DESCRIBE — получить контракт
text
SADER> DESCRIBE hash
name: hash
description: Calculate md5 and sha256 hashes for text
arguments:
  operation (string, required) - Hash algorithm: md5 | sha256
  text (string, required) - Input text to hash
CALL — выполнить операцию
text
SADER> CALL hash sha256 --text=hello
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824

SADER> CALL text length --text=hello
5

SADER> CALL model normalize --a=3,4
0.6,0.8

SADER> CALL process run --command=whoami
johnyripper

SADER> CALL http status --url=https://example.com
200
Обработка ошибок
text
SADER> CALL hash banana --text=hello
ERROR: Unknown algorithm: banana

SADER> CALL file read --path=/tmp/missing.txt
ERROR: File does not exist: /tmp/missing.txt

SADER> CALL process run --command=rm -rf /
ERROR: command not allowed: 'rm -rf /'

SADER> CALL hash sha256
ERROR: Missing required argument: text
Тесты
8 автономных тестов, 98 проверок, покрывают успешные вызовы и ошибочные сценарии:

bash
./build/test_textworker       # 10 проверок
./build/test_fileworker       # 13 проверок
./build/test_hashworker       # 9 проверок
./build/test_modelworker      # 13 проверок
./build/test_csvworker        # 13 проверок
./build/test_jsonworker       # 15 проверок
./build/test_processworker    # 15 проверок
./build/test_httpworker       # 10 проверок
Запустить все сразу:

bash
for t in textworker fileworker hashworker modelworker \
         csvworker jsonworker processworker httpworker; do
    printf "%-16s " "$t:"
    ./build/test_$t | tail -1
done
Каждый тест возвращает код 0 при успехе и 1 при провале — пригодно для CI.

Что демонстрирует проект
C++ идиомы
RAII — управление ресурсами через деструкторы: ifstream, unique_ptr с кастомными deleters для C-API (EVP_MD_CTX, CURL*, FILE* из popen)

Ownership — std::unique_ptr<Worker> в Executor, std::move для передачи владения

Rule of Zero — Command, Result, Schema, Arguments не пишут собственные деструкторы/copy/move

std::variant — типобезопасная модель команды, std::get_if для диспетчеризации

std::string_view — невладеющие представления без лишних аллокаций

Структурированные ошибки — Result::fail(...) вместо throw на границе Worker'а

const-correctness — const методы там, где состояние не меняется

Анонимные namespace — внутренние утилиты без внешней линковки

Архитектурные принципы
Единый контракт Worker'а — Executor не знает конкретных Worker'ов; добавление нового требует только addWorker(std::make_unique<NewWorker>())

Schema как единственный источник истины — описание аргументов и их валидация не могут разойтись

Границы exceptions — внутри Worker'а можно throw; на выходе всегда Result

Безопасность интерфейса — whitelist команд в ProcessWorker, проверка префикса URL в HttpWorker, запрет shell-метасимволов, лимиты размеров

Структура проекта
text
sader/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   └── sader/
│       ├── Command.h
│       ├── CommandParser.h
│       ├── Worker.h
│       ├── Executor.h
│       ├── Result.h
│       ├── Schema.h
│       ├── ArgumentSpec.h
│       ├── Arguments.h
│       ├── StringUtils.h
│       ├── HashWorker.h
│       ├── FileWorker.h
│       ├── JsonWorker.h
│       ├── HttpWorker.h
│       ├── TextWorker.h
│       ├── CsvWorker.h
│       ├── ProcessWorker.h
│       └── ModelWorker.h
├── src/
│   └── *.cpp
└── tests/
    └── test_*.cpp
Roadmap
Следующие шаги развития (по мотивам архитектурного документа курса):

□ Нормализация DISCOVER — lowercase → token search → fuzzy
□ PostgreSQL Full Text Search — индекс capability для больших каталогов
□ Embeddings / vector search — семантический поиск инструментов
□ Result как структура данных — вместо печати из Executor возвращать JSON
□ Worker Registry как отдельный компонент — вынести из Executor
□ Асинхронное выполнение — параллельные вызовы Worker'ов
□ Логирование и метрики — observability runtime'а
Контекст
Проект выполнен как сквозное учебное задание курса C++ (магистратура). Цель — на практике пройти путь от монолитного прототипа к структурированному многомодульному проекту с явными контрактами, ownership, RAII и тестами.

Автор: Serg (@molot88888888-spec)

Репозиторий: https://github.com/molot88888888-spec/sader
