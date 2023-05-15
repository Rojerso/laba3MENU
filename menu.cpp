#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <glut.h>

using namespace std;

// Класс Меню
class Menu {
private:
    string name;
    vector<Menu*> items; // вектор указателей на пункты меню
    Menu* submenu;
public:
    void addItem(Menu* i)
    {
        this->items.push_back(i);
    }
    Menu(string name) { // конструктор
        this->name = name;
    }
    void setItems(vector<Menu*> items)
    {
        this->items = items;
    }
    vector<Menu*> getItems() const
    {
        return this->items;
    }
    void setMenu(Menu* m)
    {
        this->submenu = m;
    }
    Menu* getMenu() const
    {
        return this->submenu;
    }
    string getName() const
    {
        return this->name;
    }
};


class DialogWindow {
private:
    string name;
    Menu* menu; // указатель на меню
public:
    void setMenu(Menu* m)
    {
        this->menu = m;
    }
    Menu* getMenu() const
    {
        return this->menu;
    }
    void setName(string name)
    {
        this->name = name;
    }
    string getName() const
    {
        return this->name;
    }
    DialogWindow(Menu* menu) { // конструктор
        this->menu = menu;
    }
};

void printMenu(Menu* m)
{
    cout << "Menu (" << (m->getItems()).size() << ") name:" << m->getName() << endl << "[" << endl;
    for (size_t i = 0; i < (m->getItems()).size(); i++)
    {
        cout << (m->getItems())[i]->getName() << " ";
    }
    if (m->getMenu() != nullptr)
        printMenu(m->getMenu());
    cout << endl << "]" << endl;
}

void printSystem(vector<DialogWindow> box)
{
    for (size_t i = 0; i < box.size(); i++)
    {
        cout << "Dialog:" << box[i].getName() << endl;
        printMenu(box[i].getMenu());
    }
}

// Функция чтения данных из файла
vector<DialogWindow> readDataFromFile(string filename) {
    ifstream file(filename);
    vector<DialogWindow> system;

    if (file.is_open()) {
        // считываем данные из файла и создаем объекты классов
        DialogWindow* dialogWindow = nullptr;
        Menu* menu = nullptr;
        Menu* subMenu = nullptr;
        string line;
        while (getline(file, line)) {
            if (line.find("Dialog window->(") != string::npos) {
                // создаем объект Диалоговое окно
                dialogWindow = new DialogWindow(menu);
                dialogWindow->setName(line.substr(line.find("Dialog window->(") + 16));
            }
            else if (line.find("Menu->(") != string::npos) {
                // создаем объект Меню
                menu = new Menu(line.substr(line.find("Menu->(") + 7));
            }
            else if (line.find("Submenu->(") != string::npos) {
                // создаем объект Подменю
                subMenu = new Menu(line.substr(line.find("Submenu->(") + 10));
            }
            else if (line.find("Item") != string::npos) {
                // создаем объект Пункт меню
                string name = line.substr(line.find("Item") + 5);
                Menu* item = new Menu(name);
                if (subMenu != nullptr) {
                    subMenu->addItem(item);
                }
                else {
                    menu->addItem(item);
                }
            }
            else if (line.find(")") != string::npos) {
                // закрываем объекты
                if (subMenu != nullptr) {
                    menu->setMenu(subMenu);
                    subMenu = nullptr;
                }
                else if (menu != nullptr) {
                    dialogWindow->setMenu(menu);
                    menu = nullptr;
                }
                else if (dialogWindow != nullptr)
                {
                    system.push_back(*dialogWindow);
                    dialogWindow = nullptr;
                }
            }
        }
        file.close();
    }
    else {
        cout << "Ошибка открытия файла\n";
    }
    printSystem(system);
    return system;
}



// Константы для размеров окна и элементов меню
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 480;
const int MENU_WIDTH = 208;
const int MENU_HEIGHT = 300;
const int MENU_PADDING = 10;
const int MENU_ITEM_HEIGHT = 30;
const int MAX_LENGTH_OF_NAME = 19;

vector<DialogWindow> menuItems = readDataFromFile("menu.txt");
vector<int> windows_id;
int win_ind;
Menu* currentMenu;
bool subMenuIsShown;

void drawString(const std::string& str, float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(0.15, -0.15, 1); // исправляем перевернутый текст
    for (auto c : str) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glTranslatef((WINDOW_WIDTH - MENU_WIDTH) / 2, (WINDOW_HEIGHT - MENU_HEIGHT) / 2, 0);
    // Рисуем меню
    glColor3f(0.5, 0.5, 0.5);
    glRectf(0, 0, MENU_WIDTH, MENU_HEIGHT);
    glColor3f(1, 1, 1);
    int k = 0;
    for (auto& item : currentMenu->getItems()) {
        glRectf(0, (MENU_ITEM_HEIGHT + MENU_PADDING) * k, MENU_WIDTH, (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT);
        glColor3f(0, 0, 0);
        drawString(item->getName(), (MAX_LENGTH_OF_NAME - item->getName().size() + 1) / 2 * MENU_WIDTH / (MAX_LENGTH_OF_NAME - 1), \
            (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT / 2 + 8);
        
        glColor3f(1, 1, 1);
        k++;
    }
    if (currentMenu->getMenu()->getName() != "")
    {
        glRectf(0, (MENU_ITEM_HEIGHT + MENU_PADDING) * k, MENU_WIDTH, (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT);
        glColor3f(0, 0, 0);
        drawString(currentMenu->getMenu()->getName(), (MAX_LENGTH_OF_NAME - \
            currentMenu->getMenu()->getName().size() + 1) / 2 * MENU_WIDTH / (MAX_LENGTH_OF_NAME - 1), \
            (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT / 2 + 8);

        glColor3f(1, 1, 1);
    }
    glPopMatrix();

    glFlush();
}

void createDialog();

void displaySubMenu()
{
    display();

    glPushMatrix();
    glTranslatef((WINDOW_WIDTH - MENU_WIDTH) / 2, (WINDOW_HEIGHT - MENU_HEIGHT) / 2, 0);
    int k = currentMenu->getItems().size() + 1;
    for (auto& item : currentMenu->getMenu()->getItems()) {
        glRectf(0, (MENU_ITEM_HEIGHT + MENU_PADDING) * k, MENU_WIDTH, (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT);
        glColor3f(0, 0, 0);
        drawString(item->getName(), (MAX_LENGTH_OF_NAME - item->getName().size() + 1) / 2 * MENU_WIDTH / (MAX_LENGTH_OF_NAME - 1), \
            (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT / 2 + 8);

        glColor3f(1, 1, 1);
        k++;
    }

    glPopMatrix();
    glFlush();
}

void mouse(int button, int state, int x, int y) {
    x -= (WINDOW_WIDTH - MENU_WIDTH) / 2;
    y -= (WINDOW_HEIGHT - MENU_HEIGHT) / 2;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Проверяем, было ли нажатие внутри меню
        if (x < MENU_WIDTH && y < MENU_HEIGHT) {
            int k = 0;
            // Ищем пункт меню, на который было сделано нажатие
            for (auto& item : currentMenu->getItems()) {
                if (x >= 0 && x <= MENU_WIDTH && y >= (MENU_ITEM_HEIGHT + MENU_PADDING) * k && y <= (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT) {
                    subMenuIsShown = false;
                    
                    // Действие при нажатии на пункт меню
                    cout << "Clicked on " << item->getName() << endl;
                    switch (win_ind)
                    {
                        case 0:
                            if (k == 1)
                            {
                                win_ind = 1;
                                createDialog();
                            }
                            else if (k == 2)
                            {
                                win_ind = 2;
                                createDialog();
                            }
                            else if (k == 3)
                            {
                                exit(0);
                            }
                            break;
                        case 1:
                            if (k == 0)
                            {
                                glClear(GL_COLOR_BUFFER_BIT);
                                glClearColor(1, 0, 0, 1);
                                display();
                            }
                            else if (k == 1)
                            {
                                glClear(GL_COLOR_BUFFER_BIT);
                                glClearColor(0, 1, 0, 1);
                                display();
                            }
                            else if (k == 2)
                            {
                                glClear(GL_COLOR_BUFFER_BIT);
                                glClearColor(0, 0, 0, 1);
                                display();
                            }
                            break;
                    default:
                        break;
                    }

                    break;
                }
                k++;
            }
            if (subMenuIsShown)
            {
                k = currentMenu->getItems().size() + 1;
                for (auto& item : currentMenu->getMenu()->getItems()) {
                    if (x >= 0 && x <= MENU_WIDTH && y >= (MENU_ITEM_HEIGHT + MENU_PADDING) * k && y <= (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT)
                    {
                        // Действие при нажатии на пункт меню
                        cout << "Clicked on " << item->getName() << endl;
                        switch (win_ind)
                        {
                        case 1:
                            if (k == 4)
                            {
                                win_ind = 0;
                                createDialog();
                            }
                            else if (k == 5)
                            {
                                win_ind = 3;
                                createDialog();
                            }
                            break;
                        case 2:
                            if (k == 3)
                            {
                                win_ind = 0;
                                createDialog();
                            }
                            else if (k == 4)
                            {
                                win_ind = 2;
                                createDialog();
                            }
                            break;
                        default:
                            break;
                        }

                        break;
                    }
                    k++;
                }
            }
            k = currentMenu->getItems().size();
            if (currentMenu->getMenu()->getName() != "" || subMenuIsShown)
            {
                if (x >= 0 && x <= MENU_WIDTH && y >= (MENU_ITEM_HEIGHT + MENU_PADDING) * k && y <= (MENU_ITEM_HEIGHT + MENU_PADDING) * k + MENU_ITEM_HEIGHT) {
                    // Действие при нажатии на пункт меню
                    cout << "Clicked on " << currentMenu->getMenu()->getName() << endl;
                    subMenuIsShown = true;
                    if (subMenuIsShown)
                    {
                        displaySubMenu();
                    } 
                    else
                    {
                        glClear(GL_COLOR_BUFFER_BIT);
                        display();
                        glFlush();
                    }
                }
            }
        }
    }
}

void createDialog()
{
    subMenuIsShown = false;
    glutDestroyWindow(glutGetWindow());
    glutCreateWindow((menuItems[win_ind].getName()).c_str());
    currentMenu = menuItems[win_ind].getMenu();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    win_ind = 0;
    glutCreateWindow((menuItems[win_ind].getName()).c_str());
    createDialog();

    glClearColor(0, 0, 0, 1);
    glutMainLoop();


    return 0;
}