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
     * @details Reads the username and password from the line edits and compares them 
     * using verifyUserCredentials(). If successful, it clears the input fields 
     * and switches the QStackedWidget index to the Management view.
     * @note Shows an error message via notifyStatus() if the login fails.
     */
    void handleLogin();

    /**
     * @brief Opens a file dialog to import campus data from an Excel file.
     * @details Launches a QFileDialog to select a .xlsx file. It then calls 
     * uploadFileAppend() to process the data. If new campuses are detected, 
     * it automatically triggers promptForDistances() to ensure the database 
     * maintains a complete graph of campus connections.
     */
    void on_uploadFile_clicked();

    /**
     * @brief Re-syncs the SQL models with the database.
     * @note Overrides DatabasePage::refreshUI(). This ensures that if a campus is deleted 
     * in the Admin tab, it immediately disappears from the User/Plan tabs as well.
     */
    void refreshUI() override;

private:
    Ui::AdminPage* ui;               /**< Pointer to the UI setup. */
    QSqlTableModel* campusModel;     /**< Model for the campus list table. */
    QSqlTableModel* souvenirModel;   /**< Model for the souvenirs table. */

    /**
     * @brief Prompts the user to manually input distances for newly added campuses.
     * @details Triggered after an Excel import if the file lacks specific distance mappings.
     * @param newCampusNames List of campuses that were just added to the system.
     * @return The number of distance records successfully created by the user.
     */
    int promptForDistances(const QStringList &newCampusNames);

    /**
     * @brief Initializes the SQL models and attaches them to the UI views along with the CRUD interactions
     */
    void setupDatabaseTable();

    /**
     * @brief Configures the initial state of the login screen.
     * @details Sets up validators for input fields and hides management tabs until login is successful.
     */
    void setupLoginPage();

    /**
     * @brief Static helper to check login credentials.
     * @param username The entered administrative username.
     * @param password The entered administrative password.
     * @return true if credentials match the admin records, false otherwise.
     * @note This is kept static to ensure it doesn't rely on UI state for validation.
     */
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
