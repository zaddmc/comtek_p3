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


if __name__ == "__main__":
    A = [4, 3, 2, 3]
    print("Input value is:", A)
    print("Output value is:", CountingSort(A, max(A)))
