{ causerie: A simple, elegant, and powerful programming language.
  Copyright (C) 2014 Michael Malicoat (michael@circusmachina.com).
    Home page: http://causerie.circusmachina.com
    Bug tracker: http://mantis.circusmachina.com

  This program will eventually become causerie v1.0.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
}
program cause;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  classwork;

{$R *.res}

begin
  { This program doesn't do much...yet. }
end.

