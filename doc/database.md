The server store information about the movies and show using sqlite3.

CREATE TABLE IF NOT EXISTS Movies (
    movie_id INTEGER PRIMARY KEY AUTOINCREMENT,
    title VARCHAR(255),
    genre VARCHAR(100),
    director VARCHAR(100),
    release_date DATE);


CREATE TABLE IF NOT EXISTS Shows (
    show_id INTEGER PRIMARY KEY AUTOINCREMENT,
    movie_id INT,
    nbr_seats INT,
    start_time TIME,
    end_time TIME,
    show_date DATE,
    FOREIGN KEY (movie_id) REFERENCES Movies(movie_id));


Example Dummy data

-- Inserting data into Movies table
INSERT INTO Movies (title, genre, director, release_date)
VALUES 
    ('Inception', 'Science Fiction', 'Christopher Nolan', '2010-07-16'),
    ('The Matrix', 'Action', 'Lana Wachowski', '1999-03-31'),
    ('The Shawshank Redemption', 'Drama', 'Frank Darabont', '1994-09-23'),
    ('Pulp Fiction', 'Crime', 'Quentin Tarantino', '1994-10-14'),
    ('The Godfather', 'Crime', 'Francis Ford Coppola', '1972-03-24'),
    ('Forrest Gump', 'Drama', 'Robert Zemeckis', '1994-07-06'),
    ('The Dark Knight', 'Action', 'Christopher Nolan', '2008-07-18'),
    ('Fight Club', 'Drama', 'David Fincher', '1999-10-15'),
    ('Interstellar', 'Science Fiction', 'Christopher Nolan', '2014-11-07'),
    ('Goodfellas', 'Crime', 'Martin Scorsese', '1990-09-19');

-- Inserting data into Shows table
INSERT INTO Shows (movie_id, nbr_seats, start_time, end_time, show_date) VALUES 
    (1, 100, '18:00', '21:00', '2024-01-05'),
    (2, 120, '17:30', '20:30', '2024-01-06'),
    (3, 90, '19:00', '22:00', '2024-01-07'),
    (4, 80, '18:45', '21:45', '2024-01-08'),
    (5, 150, '16:00', '19:00', '2024-01-09'),
    (6, 100, '20:15', '23:15', '2024-01-10'),
    (7, 110, '17:45', '20:45', '2024-01-11'),
    (8, 95, '18:30', '21:30', '2024-01-12'),
    (9, 85, '20:00', '23:00', '2024-01-13'),
    (10, 130, '16:45', '19:45', '2024-01-14');