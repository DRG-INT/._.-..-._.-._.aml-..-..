import { EvaluationReport, Transaction } from '../types';
export declare class AmlEngine {
    private readonly hardware;
    constructor(hardware?: string);
    evaluate(transactions: Transaction[]): EvaluationReport;
    generateSyntheticStream(count: number, fraudRatio?: number): Transaction[];
}
