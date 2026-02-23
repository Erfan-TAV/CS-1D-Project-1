#include "../headers/collegescreen.h"
#include <QApplication>
#include "../headers/tempInfo.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    collegeScreen w;

    // ASU Data
    Campus asu;
    asu.name = "Arizona State University";
    asu.items << Souvenir{"Football hfgh try rth rt hrt htrh tr htrh tr hrth Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79}
              << Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79}<< Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79}<< Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79}<< Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79}<< Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79}<< Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79}<< Souvenir{"Football Jersey", 112.44}
              << Souvenir{"Poster", 9.43}
              << Souvenir{"Sweatshirt", 42.79};

    // UCI Data
    Campus uci;
    uci.name = "University of California, Irvine (UCI)";
    uci.items << Souvenir{"Bobble head", 12.12}
              << Souvenir{"Pants", 48.88};

    // Display ASU as the default
    w.setCampusName(asu.name);
    w.populateList(asu.items);

    w.show();

    return a.exec();
}
