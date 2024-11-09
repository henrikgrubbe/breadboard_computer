export function debounceFunction<T>(fn: (args: T) => void, delay: number): (args: T) => void {
    let timeout: number | undefined;

    return (args: T) => {
        if (timeout) {
            clearTimeout(timeout);
        }

        timeout = setTimeout(() => fn.call(null, args), delay)
    }
}
