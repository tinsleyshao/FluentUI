%% run_fault_analysis.m
% 故障分析主函数包装脚本

function output = run_fault_analysis(inputs)
    try
        bus = inputs.bus_data;
        line = inputs.line_data;
        fault_bus = inputs.fault_bus;
        
        if fault_bus < 1 || fault_bus > size(bus, 1)
            error('故障母线号超出范围');
        end
        
        n_bus = size(bus, 1);
        
        % 构造导纳矩阵
        Y = zeros(n_bus, n_bus);
        
        for k = 1:size(line, 1)
            i = int32(line(k, 1));
            j = int32(line(k, 2));
            r = line(k, 3);
            x = line(k, 4);
            
            z = r + 1j*x;
            y = 1/z;
            
            Y(i,i) = Y(i,i) + y;
            Y(j,j) = Y(j,j) + y;
            Y(i,j) = Y(i,j) - y;
            Y(j,i) = Y(j,i) - y;
        end
        
        % 计算故障电流
        Y_ff = Y(fault_bus, fault_bus);
        Z_ff = 1 / Y_ff;
        
        % 假设故障点电压为0 (三相短路)
        fault_voltage = 0;
        fault_current = 1 / Z_ff;  % 故障电流
        fault_power = abs(fault_current)^2 * abs(Z_ff);
        
        % 组装输出
        output = struct;
        output.success = true;
        output.fault_voltage = fault_voltage;
        output.fault_current = fault_current;
        output.fault_power = fault_power;
        output.fault_bus = fault_bus;
        output.message = sprintf('故障分析完成: 故障母线 %d', fault_bus);
        
    catch err
        output = struct;
        output.success = false;
        output.error = err.message;
        output.error_id = err.identifier;
    end
end
