import string
import random

def gen_rand(cnt):
    with open(f"{cnt}_rand.txt", "w") as f:
        for i in range(cnt):
            len = random.randint(1, 100)
            author = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
            len = random.randint(1, 100)
            title = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
            len = random.randint(1, 100)
            publisher = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
            pages = random.randint(0, 10000)
            lit_type = random.randint(1, 3)
            f.write(author + '\n')
            f.write(title + '\n')
            f.write(publisher + '\n')
            f.write(str(pages) + '\n')
            f.write(str(lit_type) + '\n')
            if lit_type == 1:
                inner_t = random.randint(1, 3)
                f.write(str(inner_t) + '\n')
            elif lit_type == 2:
                len = random.randint(1, 100)
                industry = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
                year = random.randint(1, 10000)
                origin = random.randint(1, 2)
                f.write(industry + '\n')
                f.write(str(origin) + " " + str(year) + '\n')
            else:
                min_age = random.randint(0, 100)
                inner_t = random.randint(1, 2)
                f.write(str(inner_t) + " " + str(min_age) + '\n')
        f.close()
        
def gen_sorted(cnt, reversed):
    suffix = "rev" if reversed else "sorted"
    with open(f"{cnt}_{suffix}.txt", "w") as f:
        titles = []
        for i in range(cnt):
            len = random.randint(1, 100)
            title = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
            titles.append(title)
        titles = sorted(titles, reverse=reversed)
        for i in range(cnt):
            len = random.randint(1, 100)
            author = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
            len = random.randint(1, 100)
            publisher = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
            pages = random.randint(0, 10000)
            lit_type = random.randint(1, 3)
            f.write(author + '\n')
            f.write(titles[i] + '\n')
            f.write(publisher + '\n')
            f.write(str(pages) + '\n')
            f.write(str(lit_type) + '\n')
            if lit_type == 1:
                inner_t = random.randint(1, 3)
                f.write(str(inner_t) + '\n')
            elif lit_type == 2:
                len = random.randint(1, 100)
                industry = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(len))
                year = random.randint(1, 10000)
                origin = random.randint(1, 2)
                f.write(industry + '\n')
                f.write(str(origin) + " " + str(year) + '\n')
            else:
                min_age = random.randint(0, 100)
                inner_t = random.randint(1, 2)
                f.write(str(inner_t) + " " + str(min_age) + '\n')
        f.close()
        
def main():
    gen_rand(1000)
    gen_rand(2000)
    gen_rand(5000)
    gen_sorted(5000, False)
    gen_sorted(5000, True)
    
if __name__ == '__main__':
    main()