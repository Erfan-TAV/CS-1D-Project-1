#include "collegescreen.h"
#include <QApplication>
#include <QStringList>
#include "tempInfo.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    collegeScreen w;
    collegeScreen r;

    // Example Data for Arizona State (from your spreadsheet)
    Campus asu;
    asu.name = "Arizona State University";
    asu.items << Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79};

    // Example Data for UCI
    Campus uci;
    uci.name = "University of California, Irvine (UCI)";
    uci.items << Souvenir{"Bobble head", 12.12}
              << Souvenir{"Pants", 48.88};

    // Pass the desired campus to your window
    w.setCampusName(asu.name);
    w.populateList(asu.items);

    r.setCampusName(uci.name);
    r.populateList(uci.items);

    w.show();
    r.show();
    return a.exec();
}
