declare const process: {
  argv: string[];
  stdout: {
    write: (data: string) => boolean;
  };
  exit: (code?: number) => never;
};

declare const console: {
  log: (...args: unknown[]) => void;
  error: (...args: unknown[]) => void;
};

declare const performance: {
  now: () => number;
};
