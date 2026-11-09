def parse_line(linha):
    # Divide a string em partes usando vírgulas e remove espaços extras
    partes = [p.strip() for p in linha.split(',')]
    
    # Dicionário para armazenar os resultados
    dados = {}
    
    for parte in partes:
        # Verifica e extrai 'x'
        if parte.startswith('x ='):
            dados['x'] = int(parte.split('=')[1].strip())
            
        # Verifica e extrai 'y'
        elif parte.startswith('y ='):
            dados['y'] = int(parte.split('=')[1].strip())
            
        # Verifica e extrai 'rule'
        elif parte.startswith('rule ='):
            dados['rule'] = parte.split('=')[1].strip()
    
    return dados

entrada = input()
resultado = parse_line(entrada)

strv = list(input())
"""b2o$2ob4o$b6o$2b4o2$2b4o$b6o$2ob4o$b2o!"""
mutstr = 0
total_line = 0
y_line = 0
x_index = 0
print(1)

x_offset = 20
y_offset = 20 
print(resultado["y"] + 2*x_offset)
print(resultado["x"] + 2*y_offset) 
while True:
    if len(strv) == 0:
        strv = list(input())
    t = strv.pop(0)
    
    if t.isdigit():
        mutstr *= 10 
        mutstr += int(t)
    if t in ["o", "b"]:
        if mutstr == 0:
            mutstr = 1
        total_line += mutstr
        v = 0 
        if t == "o":
            v = 1

        for i in range(mutstr):
            if t == "o":
                print((y_offset+ y_line, x_offset + x_index + i))
        x_index += mutstr
        mutstr = 0
        
    if t == "$":
        if mutstr != 0:
            y_line += mutstr-1
        total_line = 0
        y_line += 1
        x_index = 0
        mutstr = 0
    if t == "!":
        break
alives = []

        
# for y, line in enumerate(lines):
#     for x, c in enumerate(line):
#         if c == 1:
#             print("X", end="")
#         else:
#             print(" ", end="")
#     print()