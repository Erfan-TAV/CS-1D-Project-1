/**
 * @file mainwindow.h
 * @brief Header file for the primary application window and controller logic.
 * @author Erfan Tavassoli
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include "dbManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief The main entry point and UI controller for the College Tour application.
 * @details This class manages the central QTabWidget, coordinates signal/slot 
 * connections between sub-pages (like AdminPage and InfoPage), and handles 
 * application-wide database maintenance tasks.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for MainWindow.
     * @details Initializes the UI, clears temporary trip data, establishes 
     * connections for administration signals, and sets up the initial tab state.
     * @param parent Pointer to the parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor for MainWindow.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot triggered when the user switches tabs in the main interface.
     * @details This function acts as a global refresh trigger. It finds all 
     * children of type DatabasePage and calls their refreshUI() method to 
     * ensure data consistency across the application.
     * @param index The index of the newly selected tab.
     */
    void on_tabWidget_currentChanged(int index) const;

    /**
     * @brief Resets the entire application database to its original state.
     * @details Triggered via the menu bar. It extracts a fresh master Excel 
     * file from resources to the local AppData folder and performs a full 
     * reload of the SQL tables.
     */
    void menuBarReset() const;

private:
    /**
     * @brief Connects internal signals from the AdminPage to the MainWindow's statusBar.
     * @details Allows nested widgets to display temporary status messages (like 
     * "Login Successful") without having direct access to the MainWindow pointer.
     */
    void linkAdminPage();

    /**
     * @brief Placeholder for initializing QSqlTableModels for list-based views.
     */
    void initializeList();

    Ui::MainWindow *ui;           /**< Pointer to the UI setup. */
    DbManager *dbHandler;        /**< Pointer to the global database manager. */
};

#endif // MAINWINDOW_H
