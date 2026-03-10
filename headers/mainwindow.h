/**
 * @file mainwindow.h
 * @brief Header file for the MainWindow class, the primary entry point for the UI.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dbManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
/**
 * @class MainWindow
 * @brief The main execution window of the College Tour application.
 * * 
 * * 
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
/**
     * @brief Constructs the MainWindow and initializes the UI and database connection.
     * @param parent The parent QWidget, nullptr for the main application window.
     */
    MainWindow(QWidget *parent = nullptr);
    /**
     * @brief Standard destructor to clean up UI resources and the database handler.
     */
    ~MainWindow();

private slots:
/**
     * @brief Initializes the primary lists and data views across the application tabs.
     */
    void initializeList();
    /**
     * @brief Sets up connections and permissions for the Admin interface.
     */
    void linkAdminPage();
    /**
     * @brief Resets the menu bar state to a default configuration.
     * Marked as const as it modifies the UI state without altering the class members.
     */
    void menuBarReset() const;
    /**
     * @brief Responds to tab changes to ensure the active page has the latest data.
     * @param index The index of the new selected tab.
     */
    void on_tabWidget_currentChanged(int index) const;

signals:
/**
     * @brief Internal signal emitted to notify that the database has updated.
     * This bridges the @ref DbManager notifications to the @ref DatabasePage subclasses.
     */
    void databaseChanged();


private:
    Ui::MainWindow *ui; /**< Pointer to the UI layout defined in the .ui file. */
    DbManager *dbHandler; /**< Persistent handler for all SQL database transactions. */

};
#endif // MAINWINDOW_H
