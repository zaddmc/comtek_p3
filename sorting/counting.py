"""
The algorithm given by teacher didnt really work so i stole one from wiki
https://en.wikipedia.org/wiki/Counting_sort
it runs, but gives wrong answer.
Likely due to missing function being key()
"""


def CountingSort(arr, k):

    count = [0 for _ in range(k + 1)]
    output = [0 for _ in range(len(arr))]

    for i in range(len(arr) - 1):
        j = arr[i]
        count[j] = count[j] + 1

    for i in range(1, k):
        count[i] = count[i] + count[i - 1]

    for i in reversed(range(len(arr) - 1)):
        j = arr[i]
        count[j] = count[j] - 1
        output[count[j]] = arr[i]

    return output


def counting_sort(A, n, k):
    b = [0 for _ in range(n)]
    c = [0 for _ in range(k + 1)]

    for j in range(n):
        c[A[j]] += 1

    for i in range(1, k + 1):
        c[i] += c[i - 1]

    for j in reversed(range(1, n - 1)):  # Maybe remove 1
        b[c[A[j]]] = A[j]
        c[A[j]] += -1
        print(b)

    return b


def steps_counting_sort(arr):
    max_val = max(arr)
    c = [0] * (max_val + 1)
    b = [0] * len(arr)

    print("Start")
    print("Array A:", arr, "C:", c, "B:", b)

    for i, x in enumerate(arr):
        c[x] += 1
        print(f"Count i={i}, x={x} -> C:", c)

    for i in range(1, len(c)):
        c[i] += c[i - 1]
        print(f"Cumulative i={i} -> C:", c)

    for i in range(len(arr) - 1, -1, -1):
        x = arr[i]
        c[x] -= 1
        b[c[x]] = x
        print(f"Place i={i}, x={x} -> C:", c, "B:", b)

    print("Sorted B:", b)
    return b


if __name__ == "__main__":
    if False:
        A = [4, 3, 2, 3]
        print("Input value is:", A)
        print("Output value is:", CountingSort(A, max(A)))
    if False:
        A = [8, 9, 1, 6, 5, 2, 7, 10, 16]
        print("Input value is:", A)
        print("Output value is:", counting_sort(A, len(A), max(A)))
    if True:
        A = [8, 9, 1, 6, 5, 2, 7, 10, 16]
        print("Input value is:", A)
        print("Output value is:", steps_counting_sort(A))
