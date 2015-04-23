#ifndef VRDEMOS_H_INCLUDED
#define VRDEMOS_H_INCLUDED

#include <OpenSG/OSGConfig.h>
#include <string>
#include <map>

namespace Gtk {
    class Button;
    class Image;
    class Label;
}

class VRGuiContextMenu;

OSG_BEGIN_NAMESPACE;
using namespace std;

class VRScene;
class VRSignal;

struct demoEntry {
    string path;
    string pxm_path;
    string table;
    Gtk::Button* button = 0;
    Gtk::Label* button_label = 0;
    Gtk::Image* img = 0;
    bool running = false;
    bool pixmap = false;
    bool favorite = true;
    bool write_protected = false;
};

class VRDemos {
    private:
        string active;
        VRScene* demo;
        map<string, demoEntry*> demos;
        VRGuiContextMenu* menu;

        void setButton(demoEntry* e);

        void clearTable(string t);
        void updateTable(string t);

        void setGuiState(demoEntry* e);
        void toggleDemo(demoEntry* e);
        void addEntry(string path, string table, bool running);

        void updatePixmap(demoEntry* e, Gtk::Image* img_pxb, int w, int h);
        void update();

        void initMenu();
        void on_menu_delete();
        void on_menu_advanced();
        void on_menu_unpin();

        void on_advanced_cancel();
        void on_advanced_start();

        void on_diag_new_clicked();
        void on_diag_save_clicked();
        void on_diag_load_clicked();
        void on_new_clicked();
        void on_saveas_clicked();
        void on_load_clicked();

    public:
        VRDemos();

        //VRSignal* getSignal();
};

OSG_END_NAMESPACE;

#endif // VRDEMOS_H_INCLUDED
