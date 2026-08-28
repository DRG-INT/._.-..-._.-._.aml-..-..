export type RiskSeverity = 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';
export interface Transaction {
    transaction_id: string;
    originator_account: string;
    beneficiary_account: string;
    amount: number;
    currency: string;
    timestamp: string;
    ip_address?: string;
    device_id?: string;
    country?: string;
    channel?: string;
    is_card?: boolean;
}
export interface RiskAlert {
    alert_id: string;
    rule_id: string;
    rule_name: string;
    category: string;
    severity: RiskSeverity;
    subject_account: string;
    counterparty_account?: string;
    total_suspicious_amount: number;
    currency: string;
    confidence_score: number;
    reason: string;
    typology: string;
    timestamp: string;
}
export interface EvaluationReport {
    summary: {
        total_transactions_analyzed: number;
        total_volume_analyzed: number;
        alerts_count: number;
        critical_alerts: number;
        high_alerts: number;
        medium_alerts: number;
        total_suspicious_amount: number;
        execution_time_seconds: number;
        throughput_tx_per_sec: number;
        hardware: string;
    };
    alerts: RiskAlert[];
}
