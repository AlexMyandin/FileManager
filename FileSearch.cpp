#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <fstream>
#include <Windows.h>

using namespace std;

// Функция для обрезки пробелов в начале и в конце строки
std::string trim(const std::string& str) {
    if (str.empty()) {
        return str;
    }

    // Находим первый непробельный символ с начала строки
    size_t start = str.find_first_not_of(" \t\n\r");
    // Находим первый непробельный символ с конца строки
    size_t end = str.find_last_not_of(" \t\n\r");

    // Если строка состоит только из пробелов, возвращаем пустую строку
    if (start == std::string::npos) {
        return "";
    }

    // Возвращаем подстроку без пробелов в начале и в конце
    return str.substr(start, end - start + 1);
}


class File {
protected:
    std::string name;
    //std::string creationDate;
    std::time_t creationDate;
    std::string path;
    std::vector<std::string> tags;
    double size; // Размер в килобайтах

public:
    File(const std::string& name, const std::string& creationDate2, double size, const std::string& path, const std::vector<std::string>& tags)
        : name(name), size(size), path(path), tags(tags) {
        tm date = checkDate(creationDate2);
        isValid(date.tm_mday, date.tm_mon, date.tm_year, date.tm_hour, date.tm_min);
        // Преобразуем структуру tm в количество секунд с начала эпохи
        time_t seconds = std::mktime(&date);
        creationDate = seconds;
    }

    virtual ~File() {}

    // Метод для открытия файла
    virtual void open() const {
        std::cout << "Открытие файла: " << name << std::endl;
    }
    

    std::tm checkDate(std::string date_str) {
        date_str = trim(date_str);
        size_t lenght = date_str.size();
        bool checkTime = false;
        for (size_t k = 0; k < lenght; k++) {
            if (date_str[k] == ':') { checkTime = true; break; }
        }

        size_t i = 0;
        std::string day, month, year, hour = "0", min = "0";
        for (; i < lenght; i++)
        {
            if (date_str[i] == '.') { i++; break; }
            day += date_str[i];
        }
        for (; i < lenght; i++)
        {
            if (date_str[i] == '.') { i++; break; }
            month += date_str[i];
        }
        for (; i < lenght; i++)
        {
            if (date_str[i] == ' ') { i++; break; }
            year += date_str[i];
        }
        day = trim(day); month = trim(month); year = trim(year);

        if (checkTime) {
            for (; i < lenght; i++)
            {
                if (date_str[i] == ':') { i++; break; }
                hour += date_str[i];
            }
            for (; i < lenght; i++)
            {
                min += date_str[i];
            }
        }
        int day_i = stoi(day), month_i = stoi(month), year_i = stoi(year), hour_i = stoi(hour), min_i = stoi(min);


        std::tm date = {};

        // Устанавливаем значения даты и времени
        date.tm_year = year_i - 1900;  // Год с 1900 года (например, 2023 - 1900 = 123)
        date.tm_mon = month_i;     // Месяц (0 - январь, 1 - февраль, ..., 9 - октябрь)
        date.tm_mday = day_i;    // День месяца (1-31)
        date.tm_hour = hour_i;   // Часы (0-23)
        date.tm_min = min_i;    // Минуты (0-59)
        date.tm_sec = 0;    // Секунды (0-59)

        return date;
    }

    void isValid(int day, int month, int year, int hour, int min) const {
        if (year < 1 || month < 0 || month > 11 || day < 1 || day>31 || hour < 0 || hour>23 || min < 0 || min>59) {
            throw "Неправильный ввод даты!";
        }

        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        // Проверка на високосный год
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
            daysInMonth[1] = 29;
        }

        if (!(day <= daysInMonth[month - 1])) {
            throw "Неправильный ввод даты(Проверка на високосный год)!";
        };
    }

    std::string getstrDate() const {
        tm ltm = {};
        localtime_s(&ltm, &creationDate);
        string datee = to_string(ltm.tm_mday) + "."
            + to_string(ltm.tm_mon) + "." +
            to_string(ltm.tm_year + 1900);
        return datee;
    }

    // Методы для получения информации о файле
    std::string getName() const { return name; }
    std::time_t getCreationDate() const { return creationDate; }
    double getSize() const { return size; }
    std::string getPath() const { return path; }
    std::vector<std::string> getTags() const { return tags; }

    // Метод для добавления тега
    void addTag(const std::string& tag) {
        tags.push_back(tag);
    }

    // Метод для удаления тега
    void removeTag(const std::string& tag) {
        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
    }

    // Метод для вывода информации о файле
    virtual void display() const {
        if (path != "") {
            std::cout << name << " (" << getstrDate() << "," << size << " KB," << path << ", Теги: [ ";
        }
        else {
            std::cout << name << " (" << getstrDate() << "," << size << " KB" << ", Теги: [ ";
        }
        for (const auto& tag : tags) {
            std::cout << tag << " ";
        }
        std::cout << "])" << std::endl;
    }

    // Метод для сравнения файлов по дате создания
    bool operator<(const File& other) const {
        return creationDate < other.creationDate;
    }
};


class TextDocument : public File {
public:
    TextDocument(const std::string& name, const std::string& creationDate2, double size, const std::string& path, const std::vector<std::string>& tags)
        : File(name, creationDate2, size, path, tags) {}

    void open() const override {
        std::cout << "Открытие текстового документа: " << name << std::endl;
    }
};

class PDFDocument : public File {
public:
    PDFDocument(const std::string& name, const std::string& creationDate2, double size, const std::string& path, const std::vector<std::string>& tags)
        : File(name, creationDate2, size, path, tags) {}

    void open() const override {
        std::cout << "Открытие PDF документа: " << name << std::endl;
    }
};

class ImageFile : public File {
public:
    ImageFile(const std::string& name, const std::string& creationDate2, double size, const std::string& path, const std::vector<std::string>& tags)
        : File(name, creationDate2, size, path, tags) {}

    void open() const override {
        std::cout << "Предварительный просмотр изображения: " << name << std::endl;
    }
};

class VideoFile : public File {
public:
    VideoFile(const std::string& name, const std::string& creationDate2, double size, const std::string& path, const std::vector<std::string>& tags)
        : File(name, creationDate2, size, path, tags) {}

    void open() const override {
        std::cout << "Предварительный просмотр видео: " << name << std::endl;
    }
};

class AudioFile : public File {
public:
    AudioFile(const std::string& name, const std::string& creationDate2, double size, const std::string& path, const std::vector<std::string>& tags)
        : File(name, creationDate2, size, path, tags) {}

    void open() const override {
        std::cout << "Воспроизведение аудиофайла: " << name << std::endl;
    }
};


class FileManager {
private:
    std::vector<File> files;
    std::vector<File> archive;

public:
    ~FileManager() {
  /*      for (auto file : files) {
            delete file;
        }
        for (auto file : archive) {
            delete file;
        }*/
    }


    // Метод для добавления файла
    void addFile(File file) {
        files.push_back(file);
    }

    // Метод для перемещения файла в архив
    void moveToArchive(const std::string& name) {
        auto it = std::find_if(files.begin(), files.end(), [&](File f) { return f.getName() == name; });
        if (it != files.end()) {
            archive.push_back(*it);
            files.erase(it);
            std::cout << "Файл " << name << " перемещен в архив." << std::endl;
        }
        else {
            std::cout << "Файл " << name << " не найден." << std::endl;
        }
    }

    // Метод для удаления файла
    void deleteFile(const std::string& name) {
        auto it = std::find_if(files.begin(), files.end(), [&](File f) { return f.getName() == name; });
        if (it != files.end()) {
            std::cout << "Вы уверены, что хотите удалить файл " << name << "? (y/n): ";
            char confirm;
            std::cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                //delete* it;
                files.erase(it);
                std::cout << "Файл " << name << " удален." << std::endl;
            }
            else {
                std::cout << "Удаление отменено." << std::endl;
            }
        }
        else {
            std::cout << "Файл " << name << " не найден." << std::endl;
        }
    }

    // Метод для поиска файлов по имени
    std::vector<File> searchByName(const std::string& name) const {
        std::vector<File> result;
        for (const auto& file : files) {
            if (file.getName().find(name) != std::string::npos) {
                result.push_back(file);
            }
        }
        return result;
    }

    // Метод для поиска файлов по типу
    std::vector<File> searchByType(const std::string& type) const {
        std::vector<File> result;
        for (const auto& file : files) {
            if (file.getName().find(type) != std::string::npos) {
                result.push_back(file);
            }
        }
        return result;
    }

    // Метод для поиска файлов по размеру
    std::vector<File> searchBySize(double minSize, double maxSize) const {
        std::vector<File> result;
        for (const auto& file : files) {
            if (file.getSize() >= minSize && file.getSize() <= maxSize) {
                result.push_back(file);
            }
        }
        return result;
    }

    // Метод для поиска файлов по дате создания
    std::vector<File> searchByCreationDate(std::string& date){
        std::vector<File> result;
        for (auto& file : files) {
            tm datetm = file.checkDate(date);
            file.isValid(datetm.tm_mday, datetm.tm_mon, datetm.tm_year, datetm.tm_hour, datetm.tm_min);
            // Преобразуем структуру tm в количество секунд с начала эпохи
            time_t seconds = std::mktime(&datetm);
            if (file.getCreationDate() == seconds) {
                result.push_back(file);
            }
        }
        return result;
    }

    // Метод для поиска файлов по тегам
    std::vector<File> searchByTags(const std::vector<std::string>& tags) const {
        std::vector<File> result;
        for (const auto& file : files) {
            bool match = true;
            for (const auto& tag : tags) {
                std::vector<std::string> tagsFile = file.getTags();
                //std::cout << '\n' << tagsFile[0] << '\n';
                if (std::find(tagsFile.begin(), tagsFile.end(), tag) == tagsFile.end()) {
                    match = false;
                    //file->display();
                    break;
                }
            }
            if (match) {
                //file.display();
                result.push_back(file);
            }
        }
        return result;
    }

    // Метод для сортировки файлов по имени
    void sortByName() {
        std::sort(files.begin(), files.end(), [](File a, File b) {
            return a.getName() < b.getName();
            });
    }

    // Метод для сортировки файлов по дате создания
    void sortByCreationDate() {
        std::sort(files.begin(), files.end(), [](File a, File b) {
            return a.getCreationDate() < b.getCreationDate();
            });
    }

    // Метод для сортировки файлов по размеру
    void sortBySize() {
        std::sort(files.begin(), files.end(), [](File a, File b) {
            return a.getSize() < b.getSize();
            });
    }


    std::string getstrDate(time_t seconds) const {
        tm ltm = {};
        localtime_s(&ltm, &seconds);
        string datee = to_string(ltm.tm_mday) + "."
            + to_string(ltm.tm_mon) + "." +
            to_string(ltm.tm_year + 1900);
        return datee;
    }

    // Метод для вывода статистики по найденным файлам
    void displayStatistics(const std::vector<File>& foundFiles) const {
        if (foundFiles.empty()) {
            std::cout << "Файлы не найдены." << std::endl;
            return;
        }

        double totalSize = 0;
        std::time_t lastModified = foundFiles[0].getCreationDate();

        for (const auto& file : foundFiles) {
            totalSize += file.getSize();
            if (file.getCreationDate() > lastModified) {
                lastModified = file.getCreationDate();
            }
        }
        
        std::cout << "Статистика по найденным файлам:" << std::endl;
        std::cout << "Количество файлов: " << foundFiles.size() << std::endl;
        std::cout << "Общий размер: " << totalSize << " KB" << std::endl;
        std::cout << "Средний размер: " << totalSize / foundFiles.size() << " KB" << std::endl;
        std::cout << "Последнее изменение: " << getstrDate(lastModified) << std::endl;
    }

    // Метод для вывода всех файлов
    void displayAllFiles() const {
        std::cout << "Все файлы:" << std::endl;
        for (const auto& file : files) {
            file.display();
        }
    }

    // Метод для вывода всех файлов в архиве
    void displayArchive() const {
        std::cout << "Файлы в архиве:" << std::endl;
        for (const auto& file : archive) {
            file.display();
        }
    }
};



int main() {
    SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
    SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
    FileManager fileManager;

    // Ввод данных с клавиатуры или чтение из файла
    std::ifstream inputFile("files.txt");
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::string path,name, creationDate,sizeVid, tagsStr;
            double size;
            std::vector<std::string> tags;

            std::stringstream ss(line);
            std::getline(ss, path, ',');
            path = trim(path);
            size_t pos = path.find('\\');
            if (pos != std::string::npos) {
                for (size_t i = path.size()-1; i !=-1 ; i--) {
                    if (path[i] == '\\') { break; }
                    name += path[i];
                }
            }
            else {
                name = path;
                path = "";
            }
            std::getline(ss, creationDate, ',');
            creationDate = trim(creationDate);
            ss >> size;
            ss.ignore(); // Игнорируем пробел после размера
            std::getline(ss, sizeVid, ',');

            if (sizeVid == "MB") {
                size = size * 1024;
            }
            if (sizeVid == "GB") {
                size = size * 1024 *1024;
            }
            if (sizeVid == "B") {
                size = size / 1024;
            }

            std::getline(ss, tagsStr);
            tagsStr = trim(tagsStr);
            tagsStr.erase(0,1);
            tagsStr.erase(tagsStr.size()-1,1);
            // Обработка тегов
            std::string tag;
            std::stringstream tagsStream(tagsStr);
            while (std::getline(tagsStream, tag, ',')) {
                tags.push_back(trim(tag));
            }

            // Определение типа файла по расширению
            std::string extension = name.substr(name.find_last_of(".") + 1);
            if (extension == "txt") {
                fileManager.addFile(TextDocument(name, creationDate, size, path, tags));
            }
            else if (extension == "pdf") {
                fileManager.addFile(PDFDocument(name, creationDate, size, path, tags));
            }
            else if (extension == "jpg" || extension == "png") {
                fileManager.addFile(ImageFile(name, creationDate, size, path, tags));
            }
            else if (extension == "mp4") {
                fileManager.addFile(VideoFile(name, creationDate, size, path, tags));
            }
            else if (extension == "mp3") {
                fileManager.addFile(AudioFile(name, creationDate, size, path, tags));
            }
        }
        inputFile.close();
    }
    else {
        std::cout << "Не удалось открыть файл с данными." << std::endl;
    }

    // Пример фильтрации
    cout << "Файлы без сортировки" << "\n";
    fileManager.displayAllFiles();
    cout << "\n";

    fileManager.sortByCreationDate();
    cout << "Сортировка по возрастанию даты"<<'\n';
    fileManager.displayAllFiles();
    cout << "\n";

    fileManager.sortBySize();
    cout << "Сортировка по возрастанию размера" << '\n';
    fileManager.displayAllFiles();
    cout << "\n";

    fileManager.sortByName();
    cout << "Сортировка по имени файла" << '\n';
    fileManager.displayAllFiles();
    cout << "\n";
    
    //Примеры поиска файлов
    cout << "\n" << "Поиск файлов по тегам(work)" << "\n";
    std::vector<File> foundFiles = fileManager.searchByTags({ "work" });
    for (size_t i = 0; i < foundFiles.size(); i++)
    {
        foundFiles[i].display();
    }
    fileManager.displayStatistics(foundFiles);

    cout << "\n" << "Поиск файлов по типу(pdf)" << "\n";
    vector<File> foundFilesByType = fileManager.searchByType("pdf");
    for (size_t i = 0; i < foundFilesByType.size(); i++)
    {
        foundFilesByType[i].display();
    }

    cout << "\n" << "Поиск файлов по размеру(от 100 k kb до 600 k kb)" << "\n";
    vector<File> foundFilesBySize = fileManager.searchBySize(100000,600000);
    for (size_t i = 0; i < foundFilesBySize.size(); i++)
    {
        foundFilesBySize[i].display();
    }

    cout << "\n" << "Поиск файлов по имени(Image.jpg)" << "\n";
    vector<File> foundFilesByName = fileManager.searchByName("Image.jpg");
    for (size_t i = 0; i < foundFilesByName.size(); i++)
    {
        foundFilesByName[i].display();
    }

    cout << "\n" << "Поиск файлов по имени(Image.jpg)" << "\n";
    string dateTest = "05.04.2021";
    vector<File> foundFilesByDate = fileManager.searchByCreationDate(dateTest);
    for (size_t i = 0; i < foundFilesByDate.size(); i++)
    {
        foundFilesByDate[i].display();
    }

    cout << '\n';
    // Пример открытия файла
    if (!foundFiles.empty()) {
        foundFiles[0].open();
    }

    // Пример перемещения файла в архив
    fileManager.moveToArchive("Report.pdf");
    fileManager.displayArchive();

    // Пример удаления файла
    fileManager.deleteFile("Notes.txt");

    return 0;
}