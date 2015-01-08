{ causerie: A simple, elegant, and powerful programming language.
  Copyright (C) 2014-2015 Michael Malicoat (michael@circusmachina.com).
    Home page: http://causerie.circusmachina.com
    Source code: https://github.com/circusmachina/causerie_v1
    Source code documentation:
      http://www.circusmachina.com/projects/causerie/doc/html/index.html
    Bug tracker: http://monkeywrench.circusmachina.com

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
  classwork, parsing, linearc, sysUtils;

{$R *.res}

begin
  try
    { This program doesn't do much...yet. }

  except
    // Instances of 'ALoggedItem' have already been printed
    on AE: AnException do
      if not(AE is ALoggedItem) then
        AE.print;

    on E: Exception do
      writeln(stderr, E.message);
  end;
end.

