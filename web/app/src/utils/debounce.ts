export function debounce(fn: (...args: unknown[]) => void, delay: number) {
    let timeout: number | undefined;

    return (...args: unknown[]) => {
        if (timeout) {
            clearTimeout(timeout);
        }

        timeout = setTimeout(() => fn.call(null, ...args), delay)
    }
}
