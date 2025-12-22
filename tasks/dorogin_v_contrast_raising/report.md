#Повышение контраста изображения

Студент: Дорогин Вадим Антонович, группа 3823Б1ПР3
Технология: SEQ|MPI
Вариант: 23

1. Введение

Моя задача реализовать последовательную и параллельную версии алгоритма повышения контраста изображения.
Так же написать функциональные тесты, которые проверяют корректность работы и тесты на производительность, для сравнения последовательной и параллельной реализаций.
Визуальное восприятие изображения улучшается при повышении контраста, позволяет выделить детали.


2. Постановка задачи

На вход подаем одномернный массив пикселей, с определенным размером.
На выход массив пикселей, с тем же размером, но с увеличенным контрастом.

Описание алгоритма: 

каждый элемент массива масштабируется коэффициентом kFactor = 1.3F с явным приведением типа float и ограничивается диапазоном от 0 до 255.

Краевые условия:

При пустом массиве программа не падает, происходит обработка.

Размер массива позволяет хранить в себе большое количество данных

Формула преобразования:

output[i] = std::clamp(static_cast<int>(static_cast<float>(input[i]) * kFactor), 0, 255);

3. Базовый алгоритм (Последовательный)

Алгоритм реализован в классе DoroginVContrastRaisingSEQ.

Последовательная версия проходит все элементы массива и применяет к каждому преобразование контраста.

Пример кода:

constexpr float kFactor = 1.3F;
for (std::size_t i = 0; i < input.size(); ++i) {
    int value = static_cast<int>(static_cast<float>(input[i]) * kFactor);
    output[i] = static_cast<uint8_t>(std::clamp(value, 0, 255));
}
Корректность проверяется через сравнение с эталонным массивом reference.

4. Схема распараллеливания (MPI)

Параллельная версия реализована в классе DoroginVContrastRaisingMPI.

Принцип работы:

Входной массив делится на блоки по числу процессов. Каждый процесс обрабатывает определенную свою часть массива. Функции MPI_Scatterv и MPI_Gatherv используются для распределения и сбора данных. И rank 0 собирает весь массив после обработки.

Псевдокод:

scatter input -> local_block
for each element in local_block:
    value = static_cast<int>(static_cast<float>(element) * kFactor)
    clamp value to [0, 255]
gather local_block -> output

Каждый процесс работает параллельно над своей частью массива

5 Детали реализации

Структура файлов:
tasks/dorogin_v_contrast_raising/
├───common
│   └───include
│           common.hpp
├───mpi
│   ├───include
│   │       ops_mpi.hpp
│   └───src
│           ops_mpi.cpp
├───seq
│   ├───include
│   │       ops_seq.hpp
│   └───src
│           ops_seq.cpp 
└───tests
    ├───functional
    │       main.cpp 
    └───performance
            main.cpp 

## 6. Экспериментальная настройка

Аппаратное обеспечение и ОС:
CPU: 12th Gen Intel(R) Core(TM) i7-1260U (1.10 GHz)
RAM: 16 GB
OS: Windows 10

Инструменты:
Компилятор: g++ 13.3.0 / MSVC 2022
Тип сборки: Release
MPI: MPICH / OpenMPI

Данные:
Размер массива для тестов: 50 000 000 элементов
Генерация тестовых данных: последовательность чисел от 0 до N-1

Переменные окружения MPI:
OMPI_ALLOW_RUN_AS_ROOT=1
OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1

## 7. Результаты и обсуждение

### 7.1 Корректность

Функциональные тесты выполняют проверку на совпадение выходного массива с эталонным reference
Для пустого массива отдельная проверка.

### 7.2 Производительность

| Режим | Процессы | Время, с | Ускорение | Эффективность |
| seq   | 1        | 1.141    | 1.00      | N/A           |
| mpi   | 1        | 1.088    | 1.05      | 104.8%        |
| mpi   | 2        | 0.612    | 1,87      | 93.5%         |
| mpi   | 4        | 0.323    | 3,53      | 88.3 %        |
| mpi   | 8        | 0.170    | 6,71      | 83.9 %        |

## 8. Заключение

Были реализованы последовательная и MPI версии алгоритма повышения контраста изображения.
Функциональные тесты выполнились без ошибок.
MPI-версия масштабируется, но эффективность ограничена коммуникационными накладными расходами при большом числе процессов.
В итоге - алгоритм работает без ошибок и может использоваться для обработки больших изображений.

## 9. Источники

STB Image Library: https://github.com/nothings/stb

Документация MPI: https://www.mpi-forum.org/docs/

C++ Reference: std::clamp: https://en.cppreference.com/w/cpp/algorithm/clamp

##Приложение

Пример MPI-обработки блока:

constexpr float kFactor = 1.3F;
MPI_Scatterv(input.data(), counts.data(), displs.data(), MPI_UNSIGNED_CHAR,
             local_in.data(), counts[rank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

for (std::size_t i = 0; i < local_in.size(); ++i) {
    int value = static_cast<int>(static_cast<float>(local_in[i]) * kFactor);
    local_out[i] = static_cast<uint8_t>(std::clamp(value, 0, 255));
}

MPI_Gatherv(local_out.data(), counts[rank], MPI_UNSIGNED_CHAR,
            output.data(), counts.data(), displs.data(), MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
