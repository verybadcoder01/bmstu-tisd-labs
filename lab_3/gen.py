from random import randint as rnd

def gen_perc_reg(percent, rows, cols):
    filename = f"{percent}_perc_reg.txt"
    with open(filename, 'w') as f:
        f.write(f"{rows} {cols}\n")
        for i in range(rows):
            for j in range(cols):
                val = rnd(1, 100)
                if val <= percent:
                    f.write(str(val) + ' ')
                else:
                    f.write('0 ')
            f.write('\n')
        f.write(f"{rows}\n")
        for i in range(rows):
            val = rnd(1, 100)
            if val <= percent:
                f.write(str(val) + '\n')
            else:
                f.write('0\n')


def gen_perc_sparse(percent, rows, cols):
    filename = f"{percent}_perc.txt"
    elems = []
    with open(filename, 'w') as f:
        for i in range(rows):
            for j in range(cols):
                val = rnd(1, 100)
                if val <= percent:
                    elems.append((val, i, j))
        f.write(f"{rows} {cols}\n")
        f.write(f"{len(elems)}\n")
        for elem in elems:
            f.write(f"{elem[0]} {elem[1]} {elem[2]}\n")
        f.write(f"{rows}\n")
        vec_elems = []
        for i in range(rows):
            val = rnd(1, 100)
            if val <= percent:
                vec_elems.append((val, i))
        f.write(f"{len(vec_elems)}\n")
        for vec_elem in vec_elems:
            f.write(f"{vec_elem[0]} {vec_elem[1]}\n")
            
            
def main():
    percs = [1, 10, 30, 50, 70, 80, 100]
    for perc in percs:
        gen_perc_reg(perc, 5000, 5000)
        gen_perc_sparse(perc, 5000, 5000)
        print(f"generated for {perc}")
    
    
if __name__ == '__main__':
    main()