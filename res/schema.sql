-- Create table for campus information
CREATE TABLE campusList (
                            campusID INTEGER PRIMARY KEY AUTOINCREMENT,
                            campusName TEXT NOT NULL
);

-- Create table for souvenir items
CREATE TABLE souvenirs (
                           souvenirID INTEGER PRIMARY KEY AUTOINCREMENT,
                           campusID INTEGER,
                           souvenirName TEXT NOT NULL,
                           price REAL NOT NULL,
                           FOREIGN KEY (campusID) REFERENCES campusList(campusID)
);

-- Create table for distances between campuses
CREATE TABLE campusDistances (
                                 distanceID INTEGER PRIMARY KEY AUTOINCREMENT,
                                 campusID1 INTEGER,
                                 campusID2 INTEGER,
                                 distance INTEGER NOT NULL,
                                 FOREIGN KEY (campusID1) REFERENCES campusList(campusID),
                                 FOREIGN KEY (campusID2) REFERENCES campusList(campusID)
);

-- Insert default data into campusList table
INSERT INTO campusList (campusName) VALUES ('UC Irv0000ine');
INSERT INTO campusList (campusName) VALUES ('Stanford University');
INSERT INTO campusList (campusName) VALUES ('MIT');

-- Insert default data into souvenirs table
INSERT INTO souvenirs (campusID, souvenirName, price) VALUES (1, 'Blue & Gold Hoodie', 45.99);
INSERT INTO souvenirs (campusID, souvenirName, price) VALUES (1, 'Custom Logo T-Shirt', 29.99);
INSERT INTO souvenirs (campusID, souvenirName, price) VALUES (2, 'Stanford Mug', 18.99);
INSERT INTO souvenirs (campusID, souvenirName, price) VALUES (3, 'MIT T-shirt', 24.99);

-- Insert default data into campusDistances table
INSERT INTO campusDistances (campusID1, campusID2, distance) VALUES (1, 2, 800); -- Example distance in miles
INSERT INTO campusDistances (campusID1, campusID2, distance) VALUES (1, 3, 600);
INSERT INTO campusDistances (campusID1, campusID2, distance) VALUES (2, 3, 400);