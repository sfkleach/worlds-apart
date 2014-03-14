create table world( width int, height int );
create table hex ( x int, y int, mark int, elevation int, movemod int, defmod int, primary key ( x, y )  );
create table team( x int, y int, faction int references faction );
create table faction( id int primary key, name text, color text );