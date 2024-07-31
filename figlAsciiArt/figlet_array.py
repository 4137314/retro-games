figlet_file = '/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/figlAsciiArt/retro_games.txt'
output_file = '/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/figlAsciiArt/formatted_output.c'

with open(figlet_file, 'r') as f:
    lines = f.readlines()

with open(output_file, 'w') as f:
    f.write('char *title[] = {\n')
    for line in lines:
        escaped_line = line.replace('\\', '\\\\').replace('"', '\\"').rstrip()
        f.write(f'    "{escaped_line}",\n')
    f.write('};\n')
