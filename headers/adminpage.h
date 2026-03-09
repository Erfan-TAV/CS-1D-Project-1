/**
* @file adminpage.h
 * @brief Handles logic for the adminpage.ui such as login and campus management
 * @author Erfan Tavassoli
 */
#ifndef COLLEGETOUR_ADMINPAGE_H
#define COLLEGETOUR_ADMINPAGE_H

#include <QSqlTableModel>
#include "ui_adminpage.h"
#include "databasePage.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class AdminPage;
}

QT_END_NAMESPACE

/**
 * @class AdminPage
 * @brief Provides an administrative interface to manage campuses, souvenirs, and distances.
 * * This class inherits from DatabasePage and handles the secure login process,
 * as well as CRUD (Create, Read, Update, Delete) operations on the database
 * using QSqlTableModel.
 */
class AdminPage : public DatabasePage{
    Q_OBJECT

public:
    /**
     * @brief Constructor for AdminPage.
     * @param parent Pointer to the parent widget.
     */
    explicit AdminPage(QWidget* parent = nullptr);
    /**
     * @brief Destructor for AdminPage.
     */
    ~AdminPage() override;

private slots:
    /**
     * @brief Validates administrator credentials and toggles the UI stack.
     */
    void handleLogin();
    /**
     * @brief Opens a file dialog to import campus data from an Excel file.
     */
    void on_uploadFile_clicked();
    /**
     * @brief Re-syncs the models with the database to reflect external changes.
     */
    void refreshUI() override;

private:
    Ui::AdminPage* ui;               /**< Pointer to the UI setup. */
    QSqlTableModel* campusModel;     /**< Model for the campus list table. */
    QSqlTableModel* souvenirModel;   /**< Model for the souvenirs table. */
    int promptForDistances(const QStringList &newCampusNames);
    /**
     * @brief Initializes the SQL models and attaches them to the UI views along with the CRUD interactions
     */
    void setupDatabaseTable();
    void setupLoginPage();
    static bool verifyUserCredentials(const QString &username, const QString &password);

signals:
    /**
     * @brief Emitted to request a status message update in the main window.
     * @param message The text to display.
     * @param timeout Duration in milliseconds before the message clears.
     */
    void notifyStatus(const QString &message, int timeout = 2500);
};


#endif //COLLEGETOUR_ADMINPAGE_H
