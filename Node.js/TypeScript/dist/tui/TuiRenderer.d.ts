import { EvaluationReport, RiskAlert, Transaction } from '../types';
export declare class TuiRenderer {
    static renderBanner(title: string, hardware: string): void;
    static renderProgress(current: number, total: number, elapsedSec: number, txPerSec: number, alertsCount: number, flaggedVolume: number): void;
    static renderLogEntry(tx: Transaction, alert?: RiskAlert): void;
    static renderFinalReport(report: EvaluationReport): void;
}
