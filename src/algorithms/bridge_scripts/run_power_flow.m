%% run_power_flow.m
% 潮流计算主函数包装脚本
% 输入: JSON结构体 inputs
%   - bus_data: [N×7] 母线数据矩阵
%   - line_data: [M×5] 线路数据矩阵
%   - options: 可选参数结构体
% 输出: 结果结构体

function output = run_power_flow(inputs)
    try
        % 参数提取
        bus = inputs.bus_data;
        line = inputs.line_data;
        
        % 默认参数
        if isfield(inputs, 'options')
            opts = inputs.options;
        else
            opts = struct;
        end
        
        max_iter = getfield(opts, 'max_iterations', 100);
        tol = getfield(opts, 'tolerance', 1e-6);
        
        % 数据验证
        if size(bus, 2) ~= 7 || size(line, 2) ~= 5
            error('输入数据维数不正确');
        end
        
        % 执行潮流计算
        n_bus = size(bus, 1);
        n_line = size(line, 1);
        
        % 初始化变量
        voltage = ones(n_bus, 1);  % 初始电压为1.0 pu
        P_inj = bus(:, 3) / 100;   % 有功注入
        Q_inj = bus(:, 4) / 100;   % 无功注入
        
        % 简单的迭代求解（示例）
        converged = false;
        iter = 0;
        
        for iter = 1:max_iter
            % 计算导纳矩阵
            Y = zeros(n_bus, n_bus);
            
            for k = 1:n_line
                i = int32(line(k, 1));
                j = int32(line(k, 2));
                r = line(k, 3);
                x = line(k, 4);
                b = line(k, 5);
                
                z = r + 1j*x;
                y = 1/z;
                
                Y(i,i) = Y(i,i) + y + 1j*b/2;
                Y(j,j) = Y(j,j) + y + 1j*b/2;
                Y(i,j) = Y(i,j) - y;
                Y(j,i) = Y(j,i) - y;
            end
            
            % 计算功率不平衡
            volt_complex = voltage;
            S = diag(volt_complex) * conj(Y * volt_complex);
            
            % 更新电压
            voltage_new = voltage;
            for i = 2:n_bus
                if bus(i, 2) == 1  % 负荷节点
                    S_sp = P_inj(i) + 1j*Q_inj(i);
                    S_calc = S(i);
                    dS = S_sp - S_calc;
                    
                    Y_ii = Y(i, i);
                    dV = dS / conj(Y_ii * voltage(i));
                    
                    voltage_new(i) = voltage(i) + 0.5 * dV / abs(voltage(i));
                end
            end
            
            % 检查收敛
            err = max(abs(voltage_new - voltage));
            voltage = voltage_new;
            
            if err < tol
                converged = true;
                break;
            end
        end
        
        % 计算功率流
        power_flow = zeros(n_line, 1);
        for k = 1:n_line
            i = int32(line(k, 1));
            j = int32(line(k, 2));
            
            V_i = voltage(i);
            V_j = voltage(j);
            
            % 功率计算
            S_ij = V_i * conj(Y(i,j) * (V_i - V_j));
            power_flow(k) = abs(S_ij);
        end
        
        % 计算总网损
        losses = sum(power_flow) * 0.02;  % 假设5%固定损耗率
        
        % 组装输出
        output = struct;
        output.success = true;
        output.voltage = voltage;
        output.power_flow = power_flow;
        output.losses = losses;
        output.converged = converged;
        output.iterations = iter;
        output.message = sprintf('潮流计算完成，迭代次数: %d', iter);
        
    catch err
        % 错误处理
        output = struct;
        output.success = false;
        output.error = err.message;
        output.error_id = err.identifier;
    end
end

% 辅助函数：安全获取结构体字段
function val = getfield(s, key, default)
    if isfield(s, key)
        val = s.(key);
    else
        val = default;
    end
end
