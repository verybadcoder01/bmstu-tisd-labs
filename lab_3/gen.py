from random import randint as rnd

DIR = "perf"

def gen_perc_reg(percent, rows, cols):
    filename = f"{DIR}/{percent}_perc_{rows}_reg.txt"
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
    filename = f"{DIR}/{percent}_perc_{rows}.txt"
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
    sizes = [500, 1000, 2000, 5000]
    for perc in percs:
        for size in sizes:
            gen_perc_reg(perc, size, size)
            gen_perc_sparse(perc, size, size)
            print(f"generated for {perc} and {size}")
    
    
if __name__ == '__main__':
    main()