create table world( width int, height int );
create table hex ( 
	x int, 
	y int, 
	mark int default 0, 
	elevation int default 0, 
	movemod int default 0, 
	defmod int default 0, 
	primary key ( x, y )  
);
create table team( x int, y int, faction int references faction );
create table faction( id int primary key, title text, color text );
create table goal( id integer primary key autoincrement, title text, code int default 0, x int, y int default 0 );